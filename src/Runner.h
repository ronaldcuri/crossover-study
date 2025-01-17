#ifndef RUNNER_H_
#define RUNNER_H_

#define DEBUG 1

#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <utility>
#include <core/ga/encoding.h>
#include <core/ga/ga_factory.h>
#include <core/db/base_model.hpp>
#include <core/utils/split.h>
#include <core/cli/parse.h>

// Essa classe implementa a lógica de preparação e execução do AG de forma
// genérica, ou seja, cada problema pode ser executado usando ela desde que
// sejam definidas todas as classes requisitadas
template <class ConcreteProblem, class DbModel>
class Runner {
public:
  explicit Runner(const char *instanceFilename)
    : problemInstance( new ConcreteProblem(instanceFilename) ) {}
  
  explicit Runner(std::string instanceFilename)
    : problemInstance(new ConcreteProblem(instanceFilename.c_str()) ) {}

  explicit Runner(ConcreteProblem *instance_ptr)
    : problemInstance(instance_ptr) {}

  ~Runner() = default;

  void set_problem_instance(Problem &prob) {
    this->problemInstance = &prob;
  }

  void set_problem_instance(std::string filename) {
    try {
      this->problemInstance = ConcreteProblem(filename);
    } catch (std::exception &e) {
      std::cerr << "Problem Instance Error: " << e.what() << std::endl;
      throw e;
    }
  }

  const size_t get_solution_size() {
    if (!this->wasExecuted) {
      throw std::runtime_error("Nao pode usar a função antes de executar o algoritmo");
    }
    return this->solutionSize;
  }

  const float get_solution_total_cost() {
    if (!this->wasExecuted) {
      throw std::runtime_error("Nao pode usar a função antes de executar o algoritmo");
    }
    return this->solutionTotalCost;
  }

  const std::string get_formatted_solution() {
    if (!this->wasExecuted) {
      throw std::runtime_error("Nao pode usar a função antes de executar o algoritmo");
    }
    return this->formattedSolution;
  }

  DbModel get_model() {
    if (!this->wasExecuted) {
      throw std::runtime_error("Nao pode usar a função antes de executar o algoritmo");
    }
    return this->dbModel;
  }

  void operator()(CLI *cliArguments) {
    using namespace std::chrono;
    auto filename = *(split(std::string(cliArguments->infile), '/').end()-1);
    this->dbModel = DbModel(cliArguments);

    auto population = this->problemInstance->init_pop(cliArguments->pop_size, 0.25);
    this->problemInstance->eval(population);

    this->gaFactory = new EvaluationsGAFactory(*this->problemInstance);

    GeneticAlgorithm ga = this->gaFactory->make_ga(cliArguments->tour_size,
        cliArguments->stop_criteria, cliArguments->crossover_id,
        cliArguments->crossover_rate, cliArguments->mutation_rate);

    // Executar o AG
    auto start_tpoint = std::chrono::system_clock::now();
    ga(population); /// A execucao do AG
    auto end_tpoint = system_clock::now();
    auto durationMS = duration_cast<milliseconds>(end_tpoint - start_tpoint);

    Chrom best = population.best_element();
    double finalcost = this->problemInstance->is_minimization() ?
        double(1/best.fitness()) : best.fitness();

    std::vector<int> solution;
     
    for (size_t i = 0; i < best.size(); i++) {
      if (best[i]) solution.push_back(i+1);
    }

    // Definicao da saida para o BD
    this->solutionTotalCost = finalcost;
    this->solutionSize = solution.size();
    this->formattedSolution = db::BaseModel::sequence_to_string<int>(solution);

    this->dbModel.instance_file = filename;
    this->dbModel.duration_in_ms = durationMS.count();

    // Customizando vetor de convergencia de acordo com o tipo de otimizacao
    std::vector<Chrom> convergence = ga.get_convergence();
    std::vector<double> convergenceFitness;
    if (this->problemInstance->is_minimization()) {
      for (Chrom &chrom : convergence) {
        convergenceFitness.push_back(1/chrom.fitness());
      }
    } else {
      for (Chrom &chrom : convergence) {
        convergenceFitness.push_back(chrom.fitness());
      }
    }

    this->dbModel.set_convergence(convergenceFitness);

    this->wasExecuted = true;
  }

private:
  ConcreteProblem *problemInstance;
  GAFactory *gaFactory;
  DbModel dbModel;
  size_t solutionSize;
  float solutionTotalCost;
  std::string formattedSolution;
  bool wasExecuted = false;
};

#endif 
