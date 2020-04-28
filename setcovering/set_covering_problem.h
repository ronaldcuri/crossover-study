//
// Created by ronaldd on 26/04/2020.
//

#ifndef CROSSOVERRESEARCH_SET_COVERING_PROBLEM_H
#define CROSSOVERRESEARCH_SET_COVERING_PROBLEM_H

#include <vector>
#include <ga/problem.h>
#include <scpxx/scp.h>

// TODO: Implementar classe para cobertura de conjuntos
class SetCoveringProblem : public Problem<Chrom> {
public:
    /**
     * Construtor padrão */
    SetCoveringProblem(const char *);

    /**
     * Destrutor padrão */
    ~SetCoveringProblem() = default;

    /**
     * Retorna uma referência para o atributo matriz da instância */
    const scpxx::Matrix& get_matrix();

    /**
     * Retorna um vector contendo a quantidade de linhas que cobre cada coluna*/
    std::vector<int> coverage(const Chrom&);

    /**
     * Verifica se todas as colunas estão sendo cobertas pela dada solução */
    bool check_coverage(const Chrom&);

    void display_info(std::ostream& os = std::cout) override;

    /**
     * Função objetivo do problema de cobertura de conjuntos.
     * Conta a soma dos custos das linha selecionadas e retorna com fitness */
    Fitness objective_function(Chrom&) override;


private:
    scpxx::Matrix __matrix;
};

#endif //CROSSOVERRESEARCH_SET_COVERING_PROBLEM_H