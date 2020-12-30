/*
    https://developers.google.com/optimization/assignment/assignment_example

    cd ~/c++/or-tools
    make run SOURCE=../ort/assignment1.cc

*/

#include <vector>
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/base/logging.h"

// create solver
namespace operations_research {

    void AssignmentMpi() {

        MPSolver solver ("assignment_mip", MPSolver::SAT_INTEGER_PROGRAMMING);

        // task cost for each worker

        const std::vector<std::vector<double>> costs {
            {90, 80, 75, 70},
            {35, 85, 55, 65},
            {125, 95, 90, 95},
            {45, 110, 95, 115},
            {50, 100, 90, 100}
        };

        const int num_workers = costs.size();
        const int num_tasks = costs[0].size();

        // x[i][j] is an array of 0-1 variables, which will be 1
        // if worker i is assigned to task j.
        std::vector<std::vector<const MPVariable*>> x(
            num_workers, std::vector<const MPVariable*>(num_tasks));

        for (int i = 0; i < num_workers; ++i) {
            for (int j = 0; j < num_tasks; ++j) {
                x[i][j] = solver.MakeIntVar(0, 1, "");
            }
        }

        // constraints

        // each worker can only do 1 task

        for (int i = 0; i < num_workers; ++i) {
            LinearExpr worker_sum;
            for (int j = 0; j < num_tasks; ++j) {
                worker_sum+= x[i][j];
            } 
            solver.MakeRowConstraint(worker_sum <= 1);
        }

        // each task is assigned to exactly 1 worker
        for (int j = 0; j < num_tasks; ++j) {
            LinearExpr task_sum;
            for (int i = 0; i < num_workers; ++i) {
                task_sum += x[i][j];
            }
            solver.MakeRowConstraint(task_sum == 1.0);
        }

        MPObjective* const objective = solver.MutableObjective();
        for (int i = 0; i < num_workers; ++i) {
            for (int j = 0; j < num_tasks; ++j) {
                objective->SetCoefficient(x[i][j], costs[i][j]);
            }
        }
        objective->SetMinimization();

        // Solve
        const MPSolver::ResultStatus result_status = solver.Solve();

        // print solution

        if (result_status != MPSolver::OPTIMAL & result_status != MPSolver::FEASIBLE) {
            LOG(FATAL) << "No solution found!";
        }

        LOG(INFO) << "Total cost: " << objective->Value() << "\n\n";

        for (int i = 0; i < num_workers; ++i) {
            for (int j = 0; j < num_tasks; ++j) {
                if (x[i][j]->solution_value())
                    LOG(INFO) << "Worker " << i << " assigned to task " << j << ". Cost = " << costs[i][j];
            }

        }

        
    }

}

int main(int argc, char** argv) {
    operations_research::AssignmentMpi();
    return EXIT_SUCCESS;
}