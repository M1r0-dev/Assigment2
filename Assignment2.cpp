#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <sstream>

using namespace std;

//-----------------------------------------------------
// Forward declarations for classes (do not change!)
// These declarations allow classes to reference one another.
class Fish;
class BetterFish;
class Bird;
class BetterBird;
class Mouse;
class BetterMouse;
class Monster;

//-----------------------------------------------------
// Base Animal class
// Defines the basic interface and properties for all animals.
class Animal {
private:
    const string name;  // Unique animal name (immutable)
    int daysLived;      // Number of days the animal has lived

public:
    // Constructors and destructor
    Animal() : name(""), daysLived(0) {}
    Animal(const string& name, int daysLived) : name(name), daysLived(daysLived) {}
    Animal(const Animal& other) : name(other.name), daysLived(other.daysLived) {}
    virtual ~Animal() = default;

    // Getters and setter
    int getDaysLived() const { return daysLived; }
    string getName() const { return name; }
    void setDaysLived(int newValue) { daysLived = newValue; }

    // Pure virtual functions:
    // attack: defines how an animal attacks another (sets target's daysLived to 11).
    // getType: returns the type code for the animal.
    virtual void attack(Animal& other) = 0;
    virtual string getType() const = 0;

    // Comparison operator used for sorting:
    // Animals are sorted first by daysLived and then lexicographically by name.
    bool operator<(const Animal& other) const {
        if (daysLived != other.daysLived)
            return daysLived < other.daysLived;
        return name < other.name;
    }

    // talk: Prints out the animal's information.
    void talk() const {
        cout << "My name is " << name << ", days lived: " << daysLived << endl;
    }

    // Grant friend access to derived classes to access private members if needed.
    friend class Fish;
    friend class BetterFish;
    friend class Bird;
    friend class BetterBird;
    friend class Mouse;
    friend class BetterMouse;
    friend class Monster;
};

//-----------------------------------------------------
// Concrete animal classes implementations
// Each attack method sets the target's daysLived to 11.
class Fish : virtual public Animal {
public:
    using Animal::Animal; // Inherit constructors.
    void attack(Animal& other) override {
        cout << "Fish is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "F"; }
};

class BetterFish : public Fish {
public:
    // Constructor for BetterFish with explicit values.
    BetterFish(const string& name, int daysLived) : Animal(name, daysLived) {}
    // Conversion constructor: creates a BetterFish from a Fish,
    // halving the daysLived (rounded up).
    BetterFish(const Fish& fish) : Animal(fish.getName(), (fish.getDaysLived() + 1) / 2) {}
    void attack(Animal& other) override {
        cout << "BetterFish is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "BF"; }
};

class Bird : virtual public Animal {
public:
    using Animal::Animal;
    void attack(Animal& other) override {
        cout << "Bird is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "B"; }
};

class BetterBird : public Bird {
public:
    BetterBird(const string& name, int daysLived) : Animal(name, daysLived) {}
    BetterBird(const Bird& bird) : Animal(bird.getName(), (bird.getDaysLived() + 1) / 2) {}
    void attack(Animal& other) override {
        cout << "BetterBird is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "BB"; }
};

class Mouse : virtual public Animal {
public:
    using Animal::Animal;
    void attack(Animal& other) override {
        cout << "Mouse is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "M"; }
};

class BetterMouse : public Mouse {
public:
    BetterMouse(const string& name, int daysLived) : Animal(name, daysLived) {}
    BetterMouse(const Mouse& mouse) : Animal(mouse.getName(), (mouse.getDaysLived() + 1) / 2) {}
    void attack(Animal& other) override {
        cout << "BetterMouse is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "BM"; }
};

class Monster : public BetterFish, public BetterBird, public BetterMouse {
public:
    // Monster always starts with 1 day lived.
    Monster(const string& name) : Animal(name, 1), BetterFish("", 0), BetterBird("", 0), BetterMouse("", 0) {}
    // Conversion constructor: creates a Monster from any animal.
    Monster(const Animal& animal) : Animal(animal.getName(), 1), BetterFish("", 0), BetterBird("", 0), BetterMouse("", 0) {}
    void attack(Animal& other) override {
        cout << "Monster is attacking" << endl;
        other.setDaysLived(11);
    }
    string getType() const override { return "MON"; }
};

//-----------------------------------------------------
// Container interface
// Provides a common interface for container operations.
class IContainer {
public:
    virtual ~IContainer() = default;
    virtual void addAnimal(shared_ptr<Animal> animal) = 0;
    virtual shared_ptr<Animal> getAnimalAt(int pos) = 0;
    virtual shared_ptr<Animal> removeAtIndex(int pos) = 0;
    virtual size_t size() const = 0;
    virtual void clear() = 0;
};

//-----------------------------------------------------
// Template base container class
// Implements common container functionalities and ordering for a given animal type.
template <typename T>
class Container : public IContainer {
public:
    // Reorder the container based on daysLived and name.
    void reorder() {
        sort(animals.begin(), animals.end(),
            [](const auto& a, const auto& b) {
                if(a->getDaysLived() != b->getDaysLived())
                    return a->getDaysLived() < b->getDaysLived();
                return a->getName() < b->getName();
            });
    }

    virtual ~Container() = default;

    // Pure virtual function to add an animal of type T.
    virtual void add(shared_ptr<T> animal) = 0;

    // Convert a generic Animal pointer to type T and add it.
    void addAnimal(shared_ptr<Animal> animal) override {
        auto derived = dynamic_pointer_cast<T>(animal);
        if (!derived)
            throw invalid_argument("");
        add(derived);
    }

    // Return the animal at the specified position.
    shared_ptr<T> getAnimal(int pos) {
        if (pos < 0 || pos >= animals.size())
            throw out_of_range("");
        return animals[pos];
    }

    shared_ptr<Animal> getAnimalAt(int pos) override {
        return getAnimal(pos);
    }

    // Remove and return the animal at the specified position.
    shared_ptr<T> removeAt(int pos) {
        if (pos < 0 || pos >= animals.size())
            throw out_of_range("");
        auto animal = animals[pos];
        animals.erase(animals.begin() + pos);
        return animal;
    }

    shared_ptr<Animal> removeAtIndex(int pos) override {
        return removeAt(pos);
    }

    // Clear all animals from the container.
    void clear() override { animals.clear(); }
    size_t size() const override { return animals.size(); }

protected:
    vector<shared_ptr<T>> animals; // Underlying vector storing pointers to animals of type T.
};

//-----------------------------------------------------
// Cage container – allowed for birds and mice only.
// It sorts the animals automatically upon addition.
template <typename T>
class Cage : public Container<T> {
public:
    void add(shared_ptr<T> animal) override {
        this->animals.push_back(animal);
        this->reorder();
    }
};

// Specializations to disallow Cage for Fish and BetterFish.
template <>
class Cage<Fish> {
public:
    Cage() = delete;
    Cage(const Cage<Fish>&) = delete;
    Cage& operator=(const Cage<Fish>&) = delete;
};

template <>
class Cage<BetterFish> {
public:
    Cage() = delete;
    Cage(const Cage<BetterFish>&) = delete;
    Cage& operator=(const Cage<BetterFish>&) = delete;
};

//-----------------------------------------------------
// Aquarium container – allowed for fish and mice only.
// It sorts the animals automatically upon addition.
template <typename T>
class Aquarium : public Container<T> {
public:
    void add(shared_ptr<T> animal) override {
        this->animals.push_back(animal);
        this->reorder();
    }
};

// Specializations to disallow Aquarium for Bird and BetterBird.
template <>
class Aquarium<Bird> {
public:
    Aquarium() = delete;
    Aquarium(const Aquarium<Bird>&) = delete;
    Aquarium& operator=(const Aquarium<Bird>&) = delete;
};

template <>
class Aquarium<BetterBird> {
public:
    Aquarium() = delete;
    Aquarium(const Aquarium<BetterBird>&) = delete;
    Aquarium& operator=(const Aquarium<BetterBird>&) = delete;
};

//-----------------------------------------------------
// Freedom container – accepts any animal type.
// In the Freedom container, attacks and substance operations are not allowed.
template <typename A>
class Freedom : public Container<A> {
public:
    void add(shared_ptr<A> animal) override {
        this->animals.push_back(animal);
        this->reorder();
    }
};

//-----------------------------------------------------
// Global containers for each animal type.
// These containers are pre-created according to the type of animal.
map<string, shared_ptr<Animal>> allAnimals;
Cage<Bird> cageBird;
Cage<BetterBird> cageBetterBird;
Cage<Mouse> cageMouse;
Cage<BetterMouse> cageBetterMouse;
Aquarium<Fish> aquariumFish;
Aquarium<BetterFish> aquariumBetterFish;
Aquarium<Mouse> aquariumMouse;
Aquarium<BetterMouse> aquariumBetterMouse;
Freedom<Animal> freedom;

//-----------------------------------------------------
// Helper function for updating a container during a PERIOD command.
// For each animal in the container, increases daysLived by 1.
// If the new days value exceeds 10, prints a death message and removes the animal.
// Then, the container is completely re-ordered by extracting all animals and re-adding them.
template <typename ContainerType>
void periodUpdate(ContainerType &cont) {
    int i = 0;
    // Iterate over the container while updating days and possibly removing dead animals.
    while(i < cont.size()){
        auto animal = cont.getAnimalAt(i);
        int newDays = animal->getDaysLived() + 1;
        animal->setDaysLived(newDays);
        // Special handling for Monster type animals:
        if(animal->getType() == "MON"){
            // Since Monster lives 1 day, if newDays > 0, it dies.
            if(newDays > 0) {
                cout << animal->getName() << " has died of old days" << endl;
                cont.removeAtIndex(i);
                continue; // Skip incrementing i to check new animal at this index.
            }
        }
        // For any animal, if days exceed 10, the animal dies.
        if(newDays > 10){
            cout << animal->getName() << " has died of old days" << endl;
            cont.removeAtIndex(i);
        } else {
            ++i;
        }
    }
    // Rebuild the container by extracting all animals into a temporary vector
    // and re-adding them (this ensures correct order).
    vector<shared_ptr<Animal>> temp;
    while(cont.size() > 0){
        temp.push_back(cont.removeAtIndex(0));
    }
    for(auto &a : temp)
        cont.addAnimal(a);
}

//-----------------------------------------------------
// Main function: processes commands from the console.
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C;
    cin >> C;
    string dummy;
    getline(cin, dummy); // Skip the rest of the line after reading C.

    // Process each command line by line.
    for (int i = 0; i < C; i++) {
        string line;
        getline(cin, line);
        istringstream iss(line);
        string cmd;
        iss >> cmd;

        // CREATE <TYPE> <NAME> IN <CONTAINER> <N>
        // Creates an animal of the given type with the provided name and days lived,
        // and adds it to the specified container.
        if(cmd == "CREATE"){
            string typeCode, name, inToken, containerType;
            int days;
            iss >> typeCode >> name >> inToken >> containerType >> days;
            shared_ptr<Animal> animal;
            bool flag = false;
            if(typeCode == "M")
                animal = make_shared<Mouse>(name, days);
            else if(typeCode == "F")
                animal = make_shared<Fish>(name, days);
            else if(typeCode == "B")
                animal = make_shared<Bird>(name, days);
            else if(typeCode == "BM")
                animal = make_shared<BetterMouse>(name, days);
            else if(typeCode == "BF")
                animal = make_shared<BetterFish>(name, days);
            else if(typeCode == "BB")
                animal = make_shared<BetterBird>(name, days);

            // Add the animal to the proper container.
            if(containerType == "Cage"){
                if(typeCode == "M"){
                    cageMouse.addAnimal(animal);
                    flag = true;
                } 
                else if(typeCode == "BM"){
                    cageBetterMouse.addAnimal(animal);
                    flag = true;
                }  
                else if(typeCode == "B") {
                    cageBird.addAnimal(animal);
                    flag = true;
                }
                else if(typeCode == "BB"){
                    cageBetterBird.addAnimal(animal);
                    flag = true;
                }
            }
            else if(containerType == "Aquarium"){
                if(typeCode == "F") {
                    aquariumFish.addAnimal(animal);
                    flag = true;
                }
                else if(typeCode == "BF"){
                    aquariumBetterFish.addAnimal(animal);
                    flag = true;
                }
                else if(typeCode == "M"){
                    aquariumMouse.addAnimal(animal);
                    flag = true;
                }
                else if(typeCode == "BM"){
                    aquariumBetterMouse.addAnimal(animal);
                    flag = true;
                }
            }
            else if(containerType == "Freedom"){
                freedom.addAnimal(animal);
                flag = true;
            }
            if(flag) animal->talk(); // Print the animal info after creation.
        }
        // APPLY_SUBSTANCE <CONTAINER> <TYPE> <POS>
        // Applies the substance to the animal at the given position in the specified container.
        // If applied to a normal animal, it transforms into the "better" version.
        // If applied to an already "better" animal, it becomes a Monster (and clears the container).
        else if(cmd == "APPLY_SUBSTANCE"){
            string containerType;
            iss >> containerType;
            if(containerType == "Freedom"){
                int pos;
                iss >> pos;
                cout << "Substance cannot be applied in freedom" << endl;
            } else {
                string typeCode;
                int pos;
                iss >> typeCode >> pos;
                try{
                    if(containerType == "Cage"){
                        if(typeCode == "M"){
                            auto animal = cageMouse.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "M"){
                                // Transform Mouse to BetterMouse.
                                Mouse* mPtr = dynamic_cast<Mouse*>(animal.get());
                                auto better = make_shared<BetterMouse>(*mPtr);
                                cageMouse.removeAtIndex(pos);
                                cageBetterMouse.addAnimal(better);
                            } else if(curType == "BM"){
                                // If substance applied second time, transform BetterMouse to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                cageBetterMouse.removeAtIndex(pos);
                                // Remove all animals from the BetterMouse container, printing death messages.
                                while(cageBetterMouse.size() > 0){
                                    auto a = cageBetterMouse.removeAtIndex(0);
                                    // (Note: death messages can be printed here if required by the spec)
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "B"){
                            auto animal = cageBird.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "B"){
                                // Transform Bird to BetterBird.
                                Bird* bPtr = dynamic_cast<Bird*>(animal.get());
                                auto better = make_shared<BetterBird>(*bPtr);
                                cageBird.removeAtIndex(pos);
                                cageBetterBird.addAnimal(better);
                            } else if(curType == "BB"){
                                // Transform BetterBird to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                cageBetterBird.removeAtIndex(pos);
                                while(cageBetterBird.size() > 0){
                                    auto a = cageBetterBird.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "BM"){
                            auto animal = cageBetterMouse.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "BM"){
                                // Transform BetterMouse to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                cageBetterMouse.removeAtIndex(pos);
                                while(cageBetterMouse.size() > 0){
                                    auto a = cageBetterMouse.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "BB"){
                            auto animal = cageBetterBird.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "BB"){
                                // Transform BetterBird to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                cageBetterBird.removeAtIndex(pos);
                                while(cageBetterBird.size() > 0){
                                    auto a = cageBetterBird.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                    }
                    else if(containerType == "Aquarium"){
                        if(typeCode == "F"){
                            auto animal = aquariumFish.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "F"){
                                // Transform Fish to BetterFish.
                                Fish* fPtr = dynamic_cast<Fish*>(animal.get());
                                auto better = make_shared<BetterFish>(*fPtr);
                                aquariumFish.removeAtIndex(pos);
                                aquariumBetterFish.addAnimal(better);
                            } else if(curType == "BF"){
                                // Transform BetterFish to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                aquariumBetterFish.removeAtIndex(pos);
                                while(aquariumBetterFish.size() > 0){
                                    auto a = aquariumBetterFish.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "M"){
                            auto animal = aquariumMouse.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "M"){
                                // Transform Mouse to BetterMouse.
                                Mouse* mPtr = dynamic_cast<Mouse*>(animal.get());
                                auto better = make_shared<BetterMouse>(*mPtr);
                                aquariumMouse.removeAtIndex(pos);
                                aquariumBetterMouse.addAnimal(better);
                            } else if(curType == "BM"){
                                // Transform BetterMouse to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                aquariumBetterMouse.removeAtIndex(pos);
                                while(aquariumBetterMouse.size() > 0){
                                    auto a = aquariumBetterMouse.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "BF"){
                            auto animal = aquariumBetterFish.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "BF"){
                                // Transform BetterFish to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                aquariumBetterFish.removeAtIndex(pos);
                                while(aquariumBetterFish.size() > 0){
                                    auto a = aquariumBetterFish.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                        else if(typeCode == "BM"){
                            auto animal = aquariumBetterMouse.getAnimalAt(pos);
                            string curType = animal->getType();
                            if(curType == "BM"){
                                // Transform BetterMouse to Monster.
                                auto monster = make_shared<Monster>(*animal);
                                aquariumBetterMouse.removeAtIndex(pos);
                                while(aquariumBetterMouse.size() > 0){
                                    auto a = aquariumBetterMouse.removeAtIndex(0);
                                }
                                freedom.addAnimal(monster);
                            }
                        }
                    }
                }
                catch(const out_of_range &e){
                    cout << "Animal not found" << endl;
                }
            }
        }
        // REMOVE_SUBSTANCE <CONTAINER> <TYPE> <POS>
        // Removes the substance from an animal.
        // For "better" animals, this doubles the days lived.
        // If the resulting days exceed 10, the animal dies.
        else if(cmd == "REMOVE_SUBSTANCE"){
            string containerType;
            iss >> containerType;
            if(containerType == "Freedom"){
                int pos;
                iss >> pos;
                cout << "Substance cannot be removed in freedom" << endl;
            } else {
                string typeCode;
                int pos;
                iss >> typeCode >> pos;
                try{
                    if(containerType == "Cage"){
                        if(typeCode == "BM"){
                            auto animal = cageBetterMouse.getAnimalAt(pos);
                            int newDays = animal->getDaysLived() * 2;
                            auto normal = make_shared<Mouse>(animal->getName(), newDays);
                            cageBetterMouse.removeAtIndex(pos);
                            cageMouse.addAnimal(normal);
                        }
                        else if(typeCode == "BB"){
                            auto animal = cageBetterBird.getAnimalAt(pos);
                            int newDays = animal->getDaysLived() * 2;
                            auto normal = make_shared<Bird>(animal->getName(), newDays);
                            cageBetterBird.removeAtIndex(pos);
                            cageBird.addAnimal(normal);
                        }
                        else{
                            cout << "Invalid substance removal" << endl;
                        }
                    }
                    else if(containerType == "Aquarium"){
                        if(typeCode == "BF"){
                            auto animal = aquariumBetterFish.getAnimalAt(pos);
                            int newDays = animal->getDaysLived() * 2;
                            auto normal = make_shared<Fish>(animal->getName(), newDays);
                            aquariumBetterFish.removeAtIndex(pos);
                            aquariumFish.addAnimal(normal);
                        }
                        else if(typeCode == "BM"){
                            auto animal = aquariumBetterMouse.getAnimalAt(pos);
                            int newDays = animal->getDaysLived() * 2;
                            auto normal = make_shared<Mouse>(animal->getName(), newDays);
                            aquariumBetterMouse.removeAtIndex(pos);
                            aquariumMouse.addAnimal(normal);
                        }
                        else{
                            cout << "Invalid substance removal" << endl;
                        }
                    }
                }
                catch(const out_of_range &e){
                    cout << "Animal not found" << endl;
                }
            }
        }
        // ATTACK <CONTAINER> <TYPE> <POS1> <POS2>
        // Makes the animal at POS1 attack the animal at POS2.
        // Both animals must be in the same container.
        else if(cmd == "ATTACK"){
            string containerType;
            iss >> containerType;
            if(containerType == "Freedom"){
                int pos1, pos2;
                iss >> pos1 >> pos2;
                cout << "Animals cannot attack in Freedom" << endl;
            } else {
                string typeCode;
                int pos1, pos2;
                iss >> typeCode >> pos1 >> pos2;
                if(pos1 == pos2) continue; // Prevent self-attack.
                try{
                    if(containerType == "Cage"){
                        if(typeCode == "M"){
                            auto attacker = cageMouse.getAnimalAt(pos1);
                            auto defender = cageMouse.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                cageMouse.removeAtIndex(pos2);
                        }
                        else if(typeCode == "B"){
                            auto attacker = cageBird.getAnimalAt(pos1);
                            auto defender = cageBird.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                cageBird.removeAtIndex(pos2);
                        }
                        else if(typeCode == "BM"){
                            auto attacker = cageBetterMouse.getAnimalAt(pos1);
                            auto defender = cageBetterMouse.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                cageBetterMouse.removeAtIndex(pos2);
                        }
                        else if(typeCode == "BB"){
                            auto attacker = cageBetterBird.getAnimalAt(pos1);
                            auto defender = cageBetterBird.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                cageBetterBird.removeAtIndex(pos2);
                        }
                    }
                    else if(containerType == "Aquarium"){
                        if(typeCode == "F"){
                            auto attacker = aquariumFish.getAnimalAt(pos1);
                            auto defender = aquariumFish.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                aquariumFish.removeAtIndex(pos2);
                        }
                        else if(typeCode == "BF"){
                            auto attacker = aquariumBetterFish.getAnimalAt(pos1);
                            auto defender = aquariumBetterFish.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                aquariumBetterFish.removeAtIndex(pos2);
                        }
                        else if(typeCode == "M"){
                            auto attacker = aquariumMouse.getAnimalAt(pos1);
                            auto defender = aquariumMouse.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                aquariumMouse.removeAtIndex(pos2);
                        }
                        else if(typeCode == "BM"){
                            auto attacker = aquariumBetterMouse.getAnimalAt(pos1);
                            auto defender = aquariumBetterMouse.getAnimalAt(pos2);
                            attacker->attack(*defender);
                            if(defender->getDaysLived() == 11)
                                aquariumBetterMouse.removeAtIndex(pos2);
                        }
                    }
                }
                catch(const out_of_range &e){
                    cout << "Animal not found" << endl;
                }
            }
        }
        // TALK <CONTAINER> <TYPE> <POS> or TALK Freedom <POS>
        // Prints the information of the animal at the specified position.
        else if(cmd == "TALK"){
            string containerType;
            iss >> containerType;
            if(containerType == "Freedom"){
                int pos;
                iss >> pos;
                try{
                    auto animal = freedom.getAnimalAt(pos);
                    animal->talk();
                }
                catch(const out_of_range &e){
                    cout << "Animal not found" << endl;
                }
            } else {
                string typeCode;
                int pos;
                iss >> typeCode >> pos;
                try{
                    if(containerType == "Cage"){
                        if(typeCode == "M"){
                            auto animal = cageMouse.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "BM"){
                            auto animal = cageBetterMouse.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "B"){
                            auto animal = cageBird.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "BB"){
                            auto animal = cageBetterBird.getAnimalAt(pos);
                            animal->talk();
                        }
                    }
                    else if(containerType == "Aquarium"){
                        if(typeCode == "F"){
                            auto animal = aquariumFish.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "BF"){
                            auto animal = aquariumBetterFish.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "M"){
                            auto animal = aquariumMouse.getAnimalAt(pos);
                            animal->talk();
                        }
                        else if(typeCode == "BM"){
                            auto animal = aquariumBetterMouse.getAnimalAt(pos);
                            animal->talk();
                        }
                    }
                }
                catch(const out_of_range &e){
                    cout << "Animal not found" << endl;
                }
            }
        }
        // PERIOD command: Adds +1 day to every animal.
        // This command increases each animal's age; if an animal's age exceeds 10, it dies.
        // After processing, the container is rebuilt (to maintain sorted order).
        else if(cmd == "PERIOD"){
            periodUpdate(cageBird);
            periodUpdate(cageBetterBird);
            periodUpdate(cageMouse);
            periodUpdate(cageBetterMouse);
            periodUpdate(aquariumFish);
            periodUpdate(aquariumBetterFish);
            periodUpdate(aquariumMouse);
            periodUpdate(aquariumBetterMouse);
            periodUpdate(freedom);
        }
    }
    
    return 0;
}
