#include "Chromosome.h"

class Individual {
public:
    long double fitnessValue, selectionProbability;
    bool crossesOver, willHaveMutation, isSelected, isElitist;

    Chromosome *chromosome;

    Individual(int length = 0) {
        this -> crossesOver = this -> willHaveMutation = 0;
        this -> fitnessValue = this -> selectionProbability = 0;
        this -> chromosome = new Chromosome(length);
        this -> isSelected = this -> isElitist = false;
    }

    Individual(const Individual *copy) {
        this -> fitnessValue = copy -> fitnessValue;
        this -> selectionProbability = copy -> selectionProbability;
        this -> willHaveMutation = copy -> willHaveMutation;
        this -> crossesOver = copy -> crossesOver;
        this -> chromosome = new Chromosome(copy -> chromosome);
        this -> isSelected = copy -> isSelected;
        this -> isElitist = false;
    }
};
