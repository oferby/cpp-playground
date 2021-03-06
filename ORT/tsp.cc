/* 
    https://developers.google.com/optimization/routing/tsp


*/

#include <cmath>
#include <vector>
#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

namespace operations_research {
    struct DataModel {

        const std::vector<std::vector<int64>> distance_matrix {
            {0, 2451, 713, 1018, 1631, 1374, 2408, 213, 2571, 875, 1420, 2145, 1972},
            {2451, 0, 1745, 1524, 831, 1240, 959, 2596, 403, 1589, 1374, 357, 579},
            {713, 1745, 0, 355, 920, 803, 1737, 851, 1858, 262, 940, 1453, 1260},
            {1018, 1524, 355, 0, 700, 862, 1395, 1123, 1584, 466, 1056, 1280, 987},
            {1631, 831, 920, 700, 0, 663, 1021, 1769, 949, 796, 879, 586, 371},
            {1374, 1240, 803, 862, 663, 0, 1681, 1551, 1765, 547, 225, 887, 999},
            {2408, 959, 1737, 1395, 1021, 1681, 0, 2493, 678, 1724, 1891, 1114, 701},
            {213, 2596, 851, 1123, 1769, 1551, 2493, 0, 2699, 1038, 1605, 2300, 2099},
            {2571, 403, 1858, 1584, 949, 1765, 678, 2699, 0, 1744, 1645, 653, 600},
            {875, 1589, 262, 466, 796, 547, 1724, 1038, 1744, 0, 679, 1272, 1162},
            {1420, 1374, 940, 1056, 879, 225, 1891, 1605, 1645, 679, 0, 1017, 1200},
            {2145, 357, 1453, 1280, 586, 887, 1114, 2300, 653, 1272, 1017, 0, 504},
            {1972, 579, 1260, 987, 371, 999, 701, 2099, 600, 1162, 1200, 504, 0},
        };
        const int num_vehicles = 1;
        const RoutingIndexManager::NodeIndex depot{0};

    };

    //! @brief Print the solution.
    //! @param[in] manager Index manager used.
    //! @param[in] routing Routing solver used.
    //! @param[in] solution Solution found by the solver.
    void PrintSolution(const RoutingIndexManager& manager,
        const RoutingModel& routing, const Assignment& solution) {

            LOG(INFO) << "Objective: " << solution.ObjectiveValue() << " miles";
            int64 index = routing.Start(0);
            LOG(INFO) << "Route: ";
            int64 distance{0};
            std::stringstream route;
            while (!routing.IsEnd(index)) {
                route << manager.IndexToNode(index).value() << " -> ";
                int64 priv_index = index;
                index = solution.Value(routing.NextVar(index));
                distance += routing.GetArcCostForVehicle(priv_index, index, int64{0});
            }
            LOG(INFO) << route.str() << manager.IndexToNode(index).value();
            LOG(INFO) << "Route distance: " << distance << " miles";
            LOG(INFO) << "";
            LOG(INFO) << "Advanced usage:";
            LOG(INFO) << "Problem solved in " << routing.solver()->wall_time() << "ms";

        }


    void TSP() {
        DataModel data;

        RoutingIndexManager manager(data.distance_matrix.size(), 
                                    data.num_vehicles, 
                                    data.depot);

        
        RoutingModel routing(manager);
        
        const int transit_callback_index = routing.RegisterTransitCallback(
            [&data, &manager](int64 from_index, int64 to_index) -> int64 {
                // convert from routing index to distance matrix index
                auto from_node = manager.IndexToNode(from_index).value();
                auto to_node = manager.IndexToNode(to_index).value();
                return data.distance_matrix[from_node][to_node];
            }
        );

        routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);

        // set first solution heuristic
        RoutingSearchParameters searchParameters = DefaultRoutingSearchParameters();
        searchParameters.set_first_solution_strategy(
            FirstSolutionStrategy::PATH_CHEAPEST_ARC
        );

        // solve

        const Assignment* solution = routing.SolveWithParameters(searchParameters);

        PrintSolution(manager, routing, *solution);

    }


}

int main() {
    operations_research::TSP();
    return EXIT_SUCCESS;
}

