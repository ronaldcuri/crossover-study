"""Class Command

Represents a command line program with a set of parameters.
An object of this class hold arguments and start a command line execution.
"""

import os
import sqlite3

class Command():
    problem_name : str
    args : dict
    installed_program : bool

    def __init__(self, problem, inputfile, crossover, popsize, num_epochs, database,
            crossover_rate=0.8, mutation_rate=0.05):
        self.problem_name = problem
        self.args = dict()
        self.args["infile"] = inputfile
        self.args["crossover"] = int(crossover)
        self.args["popsize"] = int(popsize)
        self.args["epochs"] = int(num_epochs)
        self.args["xrate"] = float(crossover_rate)
        self.args["mrate"] = float(mutation_rate)
        self.args["db"] = database


    def __str__(self):
        strcmd = 'crossoverstudy'
        strcmd += f' {self.problem_name} '
        for key in self.args:
            strcmd += f' --{key} {str(self.args[key])}'
        return strcmd


    def docker_command(self):
        lst = [ 'crossoverstudy', self.problem_name ]
        for key in self.args:
            lst.append(f' --{key}')
            lst.append(str(self.args[key]))
        return lst


    def get_params(self) -> dict:
        return {
            "popsize": self.args["popsize"],
            "epochs": self.args["epochs"],
            "crossover": self.args["crossover"],
            "xrate": self.args["xrate"],
            "mrate": self.args["mrate"]
        }

    
    def run(self, suppress_log=False):
        cmd_ = str(self)
        if suppress_log:
            cmd_ += " > /dev/null"
        os.system(cmd_)
