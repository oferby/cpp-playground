/*  https://developers.google.com/optimization/introduction/cpp

    example of Linear Optimization:
        
    we want to maximize 3x+y while 0 <= x <= 1 , 0 <= y <= 2 , x + y <= 2


    compile in ~/or-tools 
    make run SOURCE=../ort/linear_problem1.cc

*/
#include "ortools/linear_solver/linear_solver.h"

namespace operations_research {

    void SimpleLinearProgram() {

        LOG(INFO) << "Start";

        MPSolver* solver = MPSolver::CreateSolver("GLOP");

        const double infinity = solver->infinity();

        // set variables with limits
        
        MPVariable* const x = solver->MakeNumVar(0.0, 1, "x");
        // MPVariable* const x = solver->MakeNumVar(0.0, infinity, "x");
        MPVariable* const y = solver->MakeNumVar(0.0, 2, "y");
        

        // constraint x + y <= 2
        MPConstraint* const c0 = solver->MakeRowConstraint(-infinity, 2, "c0");
        c0->SetCoefficient(x,1);
        c0->SetCoefficient(y,1);

        LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

        // define objctive function
        // Maximize 3x + y
        MPObjective* const objective = solver->MutableObjective();
        objective->SetCoefficient(x, 3);
        objective->SetCoefficient(y, 1);
        objective->SetMaximization();

        // invoke the solver

        const MPSolver::ResultStatus result_status = solver->Solve();

        // Check for solution

        if (result_status != MPSolver::OPTIMAL) {
            LOG(FATAL) << "no optimal solution";
        }
        LOG(INFO) << "solution: " << std::endl;
        LOG(INFO) << "objective value = " << objective->Value();
        LOG(INFO) << "x = " << x->solution_value();
        LOG(INFO) << "y = " << y->solution_value();

        








    }

}


int main() {

    operations_research::SimpleLinearProgram();
    return EXIT_SUCCESS;

}
