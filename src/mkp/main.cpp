#include <iostream>
#include <chrono>
#include <paradiseo/eo/ga/eoBitOp.h>
#include <paradiseo/eo/eoDetTournamentSelect.h>
#include <paradiseo/eo/eoGenContinue.h>

#include <cli/parse.h>
#include <ga/encoding.h>
#include <ga/genetic_algorithm.h>
#include <ga/crossover_fabric.h>

#include "m_knapsack_problem.h"

using MKP = MKnapsackProblem;
using std::chrono::system_clock;

int main(int argc, char **argv) {
    cl_arguments *cli = parse(argc, argv);

    if (std::string(cli->infile).empty()) {
        std::cerr << "Falta o argumento: arquivo de instância" << std::endl;
        exit(1);
    }

    // Instância do problema
    MKnapsackProblem mkp(cli->infile);
    mkp.display_info(std::cout);
    std::cout << *cli;

    // create a population
    std::cout << "Inicializando população\n";
    auto population = mkp.init_pop(cli->pop_size);

    // evaluating the initial population
    std::cout << "Avaliando população inicial... ";
    mkp.eval(population);
    std::cout << "FEITO" << std::endl;

    // build GA components
    eoGenContinue<Chrom> continuator(cli->epochs);
    eoDetTournamentSelect<Chrom> selector(8);
    eoBitMutation<Chrom> mutator(cli->mutation_rate);

    // Choose crossover operator
    eoQuadOp<Chrom> *crossover_ptr = CrossoverFabric::create(cli->crossover_id);
    auto crossover_name = CrossoverFabric::name(cli->crossover_id);

    // Genetic Algorithm object
    GeneticAlgorithm ga(mkp, selector, *crossover_ptr, cli->crossover_rate,
            mutator, 1, continuator);
    
    
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    // std::cout << "Duração da evolução: " << ms << std::endl;

    return EXIT_SUCCESS;
}
