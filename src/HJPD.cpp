//
// Created by huan on 3/20/17.
//

#include "HJPD.h"
#include <iomanip>

using namespace std;

HJPD::HJPD(bool flag, bool svm) {
    use_test_ = flag;
    svm_output = svm;
    if (flag) {
        if (svm) out_file_name_ = "hjpd_d2.t";
        else out_file_name_ = "hjpd_d1.t";
    } else {
        if (svm) out_file_name_ = "hjpd_d2";
        else out_file_name_ = "hjpd_d1";
    }

    get_file_list();
    load_instances();
}

void HJPD::start() {
    calculate();
    write_to_file();
}

vector<double> putting_N_bins(vector<double> &data) {

    const static size_t num_bin = 13;
    //const static double bin_range[num_bin] = {-1.9, -1.7, -1.5, -1.3, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1, 0.1,
    //                                         0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 5};

    const static double bin_range[num_bin] = {-1.7, -1.3, -0.9, -0.5, -0.2, -0.1, 0.1, 0.2, 0.4, 0.8, 1.2, 1.6, 5};
    vector<double> result(num_bin, 0);

    for (double &d :data) {
        for (int i = 0; i < num_bin; ++i) {
            if (d < bin_range[i]) {
                result[i] += 1;
                break;
            }
        }
    }

    return result;
}

vector<vector<double >> compute_hist_for_dim(vector<vector<double>> &rel_dist) {
    vector<vector<double >> result;
    for (int i = 1; i <= 20; i++) {
        result.push_back(putting_N_bins(rel_dist[i]));
    }
    return result;
}

/**
 * In this method, I choose hip center as the center of the frame
 */
void HJPD::calculate() {
    for (size_t i = 0; i < positions_instances.size(); i++) {
        positions_of_instance &ins = positions_instances[i];
        relative_x_distances.clear();
        relative_y_distances.clear();
        relative_z_distances.clear();
        relative_x_distances.resize(21);
        relative_y_distances.resize(21);
        relative_z_distances.resize(21);

        size_t num_frame = ins.size();

        for (positions_of_frame &frame: ins) {
            calc_relative_displacement_to_center(frame);
        }

        vector<vector<double>> hists_x = compute_hist_for_dim(relative_x_distances);
        vector<vector<double>> hists_y = compute_hist_for_dim(relative_y_distances);
        vector<vector<double>> hists_z = compute_hist_for_dim(relative_z_distances);

        // Normalize & format output

        if (svm_output) {
            int idx = 1;
            string &filename = instance_names[i];
            size_t pos_of_file = filename.find_last_of('/');
            os_buff_ << filename.substr(pos_of_file + 2, 2) << " ";

            for (int i = 0; i < 20; i++) {
                for (double &d : hists_x[i]) {
                    d /= num_frame;
                    os_buff_ << idx++ << ":" << d << " ";
                }
                for (double &d : hists_y[i]) {
                    d /= num_frame;
                    os_buff_ << idx++ << ":" << d << " ";
                }
                for (double &d : hists_z[i]) {
                    d /= num_frame;
                    os_buff_ << idx++ << ":" << d << " ";
                }
            }
        } else {
            for (int i = 0; i < 20; i++) {
                for (double &d : hists_x[i]) {
                    d /= num_frame;
                    os_buff_ << d << " ";
                }
                for (double &d : hists_y[i]) {
                    d /= num_frame;
                    os_buff_ << d << " ";
                }
                for (double &d : hists_z[i]) {
                    d /= num_frame;
                    os_buff_ << d << " ";
                }
            }
        }

        os_buff_ << endl;
    }
}

void HJPD::calc_relative_displacement_to_center(positions_of_frame &frame) {
    calc_single_relative_displacement_to_center(frame, HIP_CENTER);
    calc_single_relative_displacement_to_center(frame, SPINE);
    calc_single_relative_displacement_to_center(frame, SHOULDER_CENTER);
    calc_single_relative_displacement_to_center(frame, HEAD);
    calc_single_relative_displacement_to_center(frame, SHOULDER_LEFT);
    calc_single_relative_displacement_to_center(frame, ELBOW_LEFT);
    calc_single_relative_displacement_to_center(frame, WRIST_LEFT);
    calc_single_relative_displacement_to_center(frame, HAND_LEFT);
    calc_single_relative_displacement_to_center(frame, SHOULDER_RIGHT);
    calc_single_relative_displacement_to_center(frame, ELBOW_RIGHT);
    calc_single_relative_displacement_to_center(frame, WRIST_RIGHT);
    calc_single_relative_displacement_to_center(frame, HAND_RIGHT);
    calc_single_relative_displacement_to_center(frame, HIP_LEFT);
    calc_single_relative_displacement_to_center(frame, KNEE_LEFT);
    calc_single_relative_displacement_to_center(frame, ANKLE_LEFT);
    calc_single_relative_displacement_to_center(frame, FOOT_LEFT);
    calc_single_relative_displacement_to_center(frame, HIP_RIGHT);
    calc_single_relative_displacement_to_center(frame, KNEE_RIGHT);
    calc_single_relative_displacement_to_center(frame, ANKLE_RIGHT);
    calc_single_relative_displacement_to_center(frame, FOOT_RIGHT);
}

void HJPD::calc_single_relative_displacement_to_center(positions_of_frame &frame, size_t posi) {
    relative_x_distances[posi].push_back(frame[posi].x - frame[HIP_CENTER].x);
    relative_y_distances[posi].push_back(frame[posi].y - frame[HIP_CENTER].y);
    relative_z_distances[posi].push_back(frame[posi].z - frame[HIP_CENTER].z);
}