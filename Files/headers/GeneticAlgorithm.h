#include "Function.h"
#include "Individual.h"
#include "RandomGenerator.h"
#include "Log.h"

#include <vector>
#include <math.h>
#include <iostream>
#include <float.h>
#include <sstream>
#include <iomanip>

template <class T>
struct GeneticAlgorithm {
    int generationCount, chromosomeSize, step, currentGeneration, precision, elitistIndex;
    double decodeNumerator, decodeDenominator, linearTranslation,
            crossoverProbability, mutationProbability;
    bool elitistCriteria;

    Log *log;

    Function <T> *fitness;
    std :: vector <Individual *> population, populationSelection, children;
    std :: vector <double> fitnessSelectionProbabilitiesIntervals;

    RandomGenerator *randomGenerator;

    GeneticAlgorithm(T *function, Domain *domain, int populationSize, int generationCount, double precision,
        double crossoverProbability, double mutationProbability, bool elitistCriteria) : generationCount(generationCount),
        step(-1), currentGeneration(0), precision(precision), crossoverProbability(crossoverProbability), mutationProbability(mutationProbability),
        elitistCriteria(elitistCriteria), population(populationSize), populationSelection(populationSize) {

        fitness = new Function <T> (function, domain);
        log = new Log();

        long long intervalCount = (domain -> b - domain -> a) * pow(10, precision),
            chromosomeSize = ceil(log2(intervalCount));

        this -> chromosomeSize = chromosomeSize;

        decodeNumerator = (fitness -> D -> b - fitness -> D -> a);
        decodeDenominator = pow(2, chromosomeSize) - 1;
        linearTranslation = decodeNumerator / decodeDenominator;

        randomGenerator = new RandomGenerator(0, 1);

        for (auto &p : population)
            p = new Individual(chromosomeSize);
    }


    /* Calculates the value coded in a chromosome */
    double decodeChromosome(Chromosome *chromosome) {
        double X10 = chromosome -> getBase10();

        return linearTranslation * X10 + fitness -> D -> a;
    }

    /* Randomises a chromosome */
    void randomizeChromosome(Chromosome *chromosome) {
        for (auto p : chromosome -> genes)
            p = randomGenerator -> getRandomInteger();
    }

    /* Calculates the fitness value of a specific individual.
        In this case, this value will be exactly f(x) */
    double calculateFitness(Individual *p) {
        return fitness -> f -> calculate(decodeChromosome(p -> chromosome));
    }

    /* Initializes population's chromosomes */
    void initializePopulationChromosomes() {
        for (Individual *p : population) {
            randomizeChromosome(p -> chromosome);
            p -> fitnessValue = calculateFitness(p);
        }

        log -> add("Population has been initialized.", "blue");
    }

    /* Finds the best individual in the current population */
    void findElitist() {
        double maxFitness = 0;

        for (size_t i = 0; i < population.size(); ++i)
            if (population[i] -> fitnessValue > maxFitness) {
                maxFitness = population[i] -> fitnessValue;
                elitistIndex = i;
            }

        population[elitistIndex] -> isElitist = true;

        log -> add("Elitist has been selected [ individual #" + std :: to_string(elitistIndex + 1) + " ].", "green");
    }

    /* Calculates a selection probability for each individual, using the formula:
            P(i) = f(i) / sum(f(i)) */
    void setSelectionProbabilities() {
        double sumFitness = 0;
        int index = 0;

        log -> add("Assigning selection probabilities to all individuals.", "blue");

        fitnessSelectionProbabilitiesIntervals = { 0 };

        for (Individual *p : population)
            sumFitness += p -> fitnessValue;

        if (elitistCriteria)
            sumFitness -= population[0] -> fitnessValue;

        for (Individual *p : population) {
            ++index;

            p -> selectionProbability = p -> fitnessValue / sumFitness;

            log -> add("Individual #" + std :: to_string(index) + ": " + std :: to_string(p -> selectionProbability), "green");

            fitnessSelectionProbabilitiesIntervals.push_back(fitnessSelectionProbabilitiesIntervals.back() + p -> selectionProbability);
        }

    }

    /* Returns the index of a random individual, based on the "Roulette Wheel" technique */
    int selectionFindIndividualIndex(double probability) {
        return lower_bound(fitnessSelectionProbabilitiesIntervals.begin(),
                                                    fitnessSelectionProbabilitiesIntervals.end(),
                                                    probability) - fitnessSelectionProbabilitiesIntervals.begin();
    }

    /* Creates a new population based on the selection probabilities */
    void selectNextIndividuals() {

        log -> add("Selecting individuals, based on their probabilities.", "blue");

        log -> add("Selection probabilities intervals:", "blue");

        std :: string probs;

        for (auto &p : fitnessSelectionProbabilitiesIntervals)
            probs += std :: to_string(p) + " | ";

        log -> add(probs.substr(0, probs.size() - 3), "green");


        if (elitistCriteria) {
            log -> add("Best individual has been chosen.", "green");
            populationSelection[0] = new Individual(population[elitistIndex]);
            populationSelection[0] -> isElitist = true;
        }

        for (auto it = populationSelection.begin() + (elitistCriteria ? 1 : 0); it != populationSelection.end(); ++it) {
            double probability = randomGenerator -> getRandomDouble();
            
            int index = selectionFindIndividualIndex(probability);

            *it = new Individual(population[index - 1]);

            population[index - 1] -> isSelected = true;

            log -> add("Choosing individual #" + std :: to_string(index) + ", with probability " + std :: to_string(probability) + ".", "green");
        }

    }

    /* Replaces the current population with the one generated by selectNextIndividuals() */
    void realiseSelection() {
        log -> add("Updating population with the latest selection.", "blue");

        for (Individual *p : population)
            delete p;

        population = populationSelection;

        for (Individual *p : population)
            p -> selectionProbability = p -> isSelected = 0;
    }

    /* Selects random individuals for crossing over */
    void setCrossoverIndividuals() {
        int index = 0;
        bool exists = false;

        log -> add("Selecting individuals that will crossover.", "blue");

        for (Individual *p : population) {
            ++index;

            if (p -> isElitist)
                continue;

            double probability = randomGenerator -> getRandomDouble();

            if (probability < crossoverProbability) {
                p -> crossesOver = true;
                exists = true;

                log -> add("Choosing individual #" + std :: to_string(index) +", with probability " +
                                                   std :: to_string(probability) + " < " + std :: to_string(crossoverProbability), "green");
            }
        }

        if (!exists)
            log -> add("No individuals were chosen for crossing over.", "blue");
    }

    /* Recombines every other two individuals that were chosen by setCrossoverIndividuals().
        If there are less than two individuals, nothing will be done.
        In case of an odd number of individuals, the last three parents will be replaced by
        the first two parents' children.
    */
    void recombineIndividuals() {
        Individual *father = nullptr, *mother = nullptr, *child = nullptr;

        int index = 0, indexFather = -1, indexMother = -1;

        log -> add("Recombining individuals.", "blue");

        static RandomGenerator *randomGeneratorBP = new RandomGenerator(0, chromosomeSize);

        for (Individual *p : population) {
            ++index;

            if (!p -> crossesOver)
                continue;

            if (father == nullptr && mother == nullptr) {
                father = p;
                indexFather = index;
                continue;
            }

            if (mother == nullptr) {
                mother = p;
                indexMother = index;
            }

            log -> add("Recombining individual #" + std :: to_string(indexFather) + " and individual #" + std :: to_string(indexMother) + " chromosomes.",
                                  "green");

            child = new Individual();

            int breakPoint = randomGeneratorBP -> getRandomInteger();

            child -> chromosome -> genes.insert(child -> chromosome -> genes.end(), 
                                                father -> chromosome -> genes.begin(),
                                                father -> chromosome -> genes.begin() + breakPoint);
            
            child -> chromosome -> genes.insert(child -> chromosome -> genes.end(),
                                                mother -> chromosome -> genes.begin() + breakPoint,
                                                mother -> chromosome -> genes.end());

            child -> fitnessValue = calculateFitness(child);

            *father = new Individual(child);
            *mother = new Individual(child);

            father = nullptr;
            mother = nullptr;
        }

        if (indexMother == -1) {

            std :: string message;

            if (indexFather == -1)
                message = "No";
            else
                message = "Only one";

            message += " individual selected, can not combine.";

            log -> add(message, "blue");

            return;
        }

        if (father && child) {
            log -> add("Recombining last remaining individual with previous parents.", "green");

            *father = new Individual(child);
        }

        delete child;
    }

    /* Selects random individuals for mutation */
    void setMutationsIndividuals() {
        int index = 0;
        bool exists = false;

        log -> add("Selecting individuals that will be mutated.", "blue");

        for (Individual *p : population) {
            double probability = randomGenerator -> getRandomDouble();
            ++index;

            if (p -> isElitist)
                continue;

            if (probability < mutationProbability) {
                p -> willHaveMutation = true;
                exists = true;

               log -> add("Choosing individual #" + std :: to_string(index) + ".", "green");

            }
        }

        if (!exists)
            log -> add("No individuals were chosen for mutating.", "blue");
    }

    /* Applies a rare mutation to each individual selected by setMutationsIndividuals() */
    void mutateIndividuals() {
        static RandomGenerator *randomGeneratorMI = new RandomGenerator(0, chromosomeSize);

        log -> add("Mutating selected individuals.", "blue");

        for (Individual *p : population) {
            if (!p -> willHaveMutation)
                continue;

            int index = randomGeneratorMI -> getRandomInteger();

            p -> chromosome -> genes[index] = !p -> chromosome -> genes[index];

            p -> willHaveMutation = false;
        }
    }

    /* Normalizes the population, as if it were the first one */
    void normalizeIndividuals() {
        for (Individual *p : population) {
            p -> fitnessValue = fitness -> f -> calculate(decodeChromosome(p -> chromosome));
            p -> crossesOver = p -> isSelected = p -> isElitist = false;
            p -> selectionProbability = 0;
        }
    }

    /* Stringifies the current population */
    std :: vector <std :: string> populationToString() {
        static std :: ostringstream streamX, streamFX;

        std :: vector <std :: string> ans;
        int index = 0;

        for (Individual *p : population) {
            streamX << std :: setprecision(precision);
            streamFX << std :: setprecision(precision);
            streamX << decodeChromosome(p -> chromosome);
            streamFX << p -> fitnessValue;
            std :: string current = std :: to_string(++index) + " ";
            current += p -> chromosome -> toString() + " ";
            current += streamX.str() + " ";
            current += streamFX.str() + " ";
            current += std :: to_string(p -> selectionProbability) + " ";
            current += std :: to_string(p -> isSelected) + " ";
            current += std :: to_string(p -> crossesOver) + " ";
            current += std :: to_string(p -> willHaveMutation) + " ";
            current += std :: to_string(p -> isElitist) + " ";

            ans.push_back(current);
            streamX.str("");
            streamFX.str("");
        }

        return ans;
    }

    /* Based on the current step, executes the next step in the genetic algorithm */
    void nextStep() {
        switch(step) {
            case -1:
                initializePopulationChromosomes();
                break;
            case 0: {
                if (elitistCriteria)
                    findElitist();

                setSelectionProbabilities();
                break;
            }
            case 1:
                selectNextIndividuals();
                break;
            case 2:
                realiseSelection();
                break;
            case 3:
                setCrossoverIndividuals();
                break;
            case 4:
                recombineIndividuals();
                break;
            case 5:
                setMutationsIndividuals();
                break;
            case 6:
                mutateIndividuals();
                normalizeIndividuals();
                ++currentGeneration;
                break;

        }

        ++step;

        if (step == 7)
            step = 0;

    }

    /* Executes a whole cycle of steps */
    void nextCycle() {
        if (!step)
            nextStep();

        while (step)
           nextStep();
    }

    /* Runs the algorithm for a given number of times [ generationCount, parameter ] */
    void runAll() {
        log -> keepLog = false;

        while (currentGeneration < generationCount)
            nextCycle();

        log -> keepLog = true;
    }
};
