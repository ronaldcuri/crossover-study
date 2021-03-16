#ifndef CORE_GA_GA_FACTORY_H_
#define CORE_GA_GA_FACTORY_H_

#include <cstdint>
#include <chrono>
#include <paradiseo/eo/eoOp.h>
#include <paradiseo/eo/eoSelectOne.h>
#include <paradiseo/eo/eoDetTournamentSelect.h>
#include <paradiseo/eo/eoGenContinue.h>
#include <paradiseo/eo/eoTimeContinue.h>
#include "encoding.h"
#include "genetic_algorithm.h"
#include "crossover_fabric.h"

/**
 * @class GAFabric
 * Essa classe deve criar uma uma instância de algoritmo genético dados os
 * parâmetros estabelecidos de forma acessível de fora da classe GA.
 */
class GAFactory {
public:

  virtual ~GAFactory() = default;

  virtual GeneticAlgorithm make_ga(
      uint8_t tourRingSize,
      uint32_t stop,
      uint8_t crossoverId, 
      float crossRate, 
      float mutRate) = 0;
};

/**
 * Cria um Algoritmo Genetico com o numero de geracoes como criterio de parada */
class GenerationsGAFactory : public GAFactory {
public:
  explicit GenerationsGAFactory(Problem &problem_) : problem(problem_) {}

  ~GenerationsGAFactory() = default;

  GeneticAlgorithm make_ga(uint8_t tourRingSize, uint32_t stop,
      uint8_t crossoverId, float crossRate, float mutRate)
  {
    select = eoDetTournamentSelect<Chrom>(tourRingSize);
    stopCriteria = new eoGenContinue<Chrom>(stop);
    mutationOp = eoBitMutation<Chrom>(mutRate);
    crossoverPtr = CrossoverFabric::create(crossoverId);

    GeneticAlgorithm ga(problem,
        select, *crossoverPtr, crossRate, mutationOp, 1.0F, *stopCriteria);
    return ga;
  }

private:
  Problem &problem;
  eoDetTournamentSelect<Chrom> select;
  eoQuadOp<Chrom> *crossoverPtr;
  eoBitMutation<Chrom> mutationOp;
  eoGenContinue<Chrom> *stopCriteria;
};

/**
 * Cria um Algoritmo Genetico com um tempo como criterio de parada */
class TimeGAFactory : public GAFactory {
public:
  explicit TimeGAFactory(Problem &problem_) : problem(problem_) {}

  ~TimeGAFactory() = default;

  GeneticAlgorithm make_ga(uint8_t tourRingSize, uint32_t stop,
      uint8_t crossoverId, float crossRate, float mutRate)
  {
    std::chrono::duration<uint32_t, std::ratio<1>> _time(stop);
    stopCriteria = new eoTimeContinue<Chrom>(_time.count());

    select = eoDetTournamentSelect<Chrom>(tourRingSize);
    mutationOp = eoBitMutation<Chrom>(mutRate);
    crossoverPtr = CrossoverFabric::create(crossoverId);

    GeneticAlgorithm ga(problem,
        select, *crossoverPtr, crossRate, mutationOp, 1.0F, *stopCriteria);
    return ga;
  }

private:
  Problem &problem;
  eoDetTournamentSelect<Chrom> select;
  eoQuadOp<Chrom> *crossoverPtr;
  eoBitMutation<Chrom> mutationOp;
  eoTimeContinue<Chrom> *stopCriteria;
};

#endif
