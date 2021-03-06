//
// Created by joonho on 7/28/20.
//

#include <environment/environment_c100.hpp>
#include <graph/Policy.hpp>

int main(int argc, char *argv[]) {

  ///Hard-coded. TODO: clean it up
  int history_len = 100; // todo: move to controller config.
  std::string rsc_path = "/home/jolee/raisimws/learning_locomotion_over_challening_terrain_supplementary/rsc";

  std::string urdf_path = rsc_path + "/robot/c100/urdf/anymal_minimal.urdf";
  std::string actuator_path = rsc_path + "/actuator/C100/seaModel_2500.txt";
  std::string network_path = rsc_path + "/controller/c100/graph.pb";
  std::string param_path = rsc_path + "/controller/c100/param.txt";

  raisim::World::setActivationKey("/home/jolee/raisimws/install/activation.raisim");

  Env::blind_locomotion sim(true, 0, urdf_path, actuator_path);

  Policy<Env::ActionDim> policy;

  policy.load(network_path,
              param_path,
              Env::StateDim,
              Env::ObservationDim,
              history_len);

  Eigen::Matrix<float, -1, -1> history;
  Eigen::Matrix<float, -1, 1> state;
  Eigen::Matrix<float, Env::ActionDim, 1> action;

  /// set terrain properties
  Eigen::Matrix<float, -1, 1> task_params(4);
  task_params << 0.0, 0.05, 0.5, 0.5;
  sim.updateTask(task_params);
  sim.setFootFriction(0, 0.1);

  sim.init();
  /// simulate for 30 seconds.
  for (int i = 0; i < 1500; i++) {
    sim.integrate();
    sim.getHistory(history, history_len);
    sim.getState(state);

    policy.updateStateBuffer(state);
    policy.updateStateBuffer2(history);
    policy.getAction(action);
    sim.updateAction(action);
  }
  return 0;
}