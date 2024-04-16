#include "Eigen/Dense"

#define N_STATES (3)
#define N_INPUTS (1)
extern "C"
{
    void DARE_Eigen_Decomp()
    {
        Eigen::MatrixXd A(N_STATES, N_STATES);
        Eigen::MatrixXd B(N_STATES, N_INPUTS);
        Eigen::MatrixXd Q(N_STATES, N_STATES);
        Eigen::MatrixXd R(N_INPUTS, N_INPUTS);

        A << 1, -0.0303954624728227, -0.000759886561820567,
            0, 1.01190488946852, 0.0502976222367131,
            0, 0.476195578740889, 1.01190488946852;
        B << 0.0470730639665003,
            -0.00107583894243483,
            -0.0430335576973931;

        Q << 1, 0, 0,
            0, 1, 0,
            0, 0, 1;
        R << 1;

        Eigen::MatrixXd B_R_inv_Bt = B * R.inverse() * B.transpose();
        Eigen::MatrixXd A_t_inv = A.transpose().inverse();

        // Calculate the components of the Hamiltonian matrix
        Eigen::MatrixXd H11 = A + B_R_inv_Bt * A_t_inv * Q;
        Eigen::MatrixXd H12 = -B_R_inv_Bt * A_t_inv;
        Eigen::MatrixXd H21 = -A_t_inv * Q;
        Eigen::MatrixXd H22 = A_t_inv;

        // Assemble the Hamiltonian matrix
        Eigen::MatrixXd HamiltonianMatrix(A.rows() * 2, A.cols() * 2);
        HamiltonianMatrix << H11, H12,
            H21, H22;

        // Solve the eigenproblem for the Hamiltonian matrix
        Eigen::EigenSolver<Eigen::MatrixXd> eigen_solver(HamiltonianMatrix);
        Eigen::MatrixXd stable_eigen_vectors(N_STATES * 2, N_STATES);
        int stable_eigen_counter = 0;
        std::vector<int> stable_eigen_indices = {};
        // Iterate through eigenvalues and rearrange eigenvectors
        for (int i = 0; i < eigen_solver.eigenvalues().size(); i++)
        {
            if (stable_eigen_counter == A.rows())
            {
                break;
            }
            // Check if eigenvalue length or norm is less than 1
            if (std::abs(eigen_solver.eigenvalues()[i]) < 1)
            {
                stable_eigen_counter++;
                // Get corresponding eigenvector
                Eigen::VectorXcd eigenvector = eigen_solver.eigenvectors().col(i);
                stable_eigen_indices.push_back(i);
            }
        }
        stable_eigen_vectors << eigen_solver.eigenvectors().col(stable_eigen_indices[0]).real(), eigen_solver.eigenvectors().col(stable_eigen_indices[1]).real(), eigen_solver.eigenvectors().col(stable_eigen_indices[2]).real();

        // Create the upper and lower matrices
        Eigen::MatrixXd upper_matrix = stable_eigen_vectors.topRows(N_STATES);
        Eigen::MatrixXd lower_matrix = stable_eigen_vectors.bottomRows(N_STATES);

        Eigen::MatrixXd S = lower_matrix * upper_matrix.inverse();

        Eigen::MatrixXd K1 = -(R + B.transpose() * S * B).inverse();
        Eigen::MatrixXd K2 = (B.transpose() * S * A);

        Eigen::MatrixXd K = K1 * K2;
    }
}