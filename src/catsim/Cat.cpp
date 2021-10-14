//
// Created by isura on 10/12/2021.
//

#include "Cat.h"

#include <exception>
#include <cmath>

#include "Config.h"
#include "Utils.h"

CatSummary::CatSummary()
        : state_hours(), attacked(), got_attacked(), conceived(),
          delivered(), lost_health(), consumed_food(), moved(), aged() {
    for (const auto &state: Enum<State>::all) {
        this->state_hours[state] = 0.0;
    }
}

void CatSummary::update_moved(double distance) {
    this->moved += distance;
}

void CatSummary::update_conceived() {
    ++this->conceived;
}

void CatSummary::update_attacked(double amount) {
    this->attacked += amount;
}

void CatSummary::update_got_attacked(double amount) {
    this->got_attacked += amount;
}

void CatSummary::update_delivered() {
    ++this->delivered;
}

void CatSummary::update_consumed_food(double amount) {
    this->consumed_food += amount;
}

void CatSummary::update_lost_health(double amount) {
    this->lost_health += amount;
}

void CatSummary::update_aged(double years) {
    this->aged += years;
}

void CatSummary::update_state_hours(State state, size_t hours) {
    this->state_hours[state] += hours;
}

std::ostream &operator<<(std::ostream &os, const CatSummary &summary) {
    os << "Summary{";
    os << "state_hours=" << summary.state_hours << ", ";
    os << "attacked=" << summary.attacked << ", ";
    os << "got_attacked=" << summary.got_attacked << ", ";
    os << "conceived=" << summary.conceived << ", ";
    os << "delivered=" << summary.delivered << ", ";
    os << "lost_health=" << summary.lost_health << ", ";
    os << "consumed_food=" << summary.consumed_food << ", ";
    os << "moved=" << summary.moved << ", ";
    os << "aged=" << summary.aged;
    os << "}";
    return os;
}

std::vector<Cat *> Cat::all;
int Cat::_next_id = 0;

Cat::Cat(const Vec2 &position, double age, Gender gender, Personality personality, double health, State state,
         size_t sleep_duration, Cat *fetus, size_t hours_since_last_conception, int cat_id)
        : cat_id(cat_id >= 0 ? std::max(Cat::_next_id, cat_id) : Cat::_next_id),
          position(position), age(age), gender(gender), personality(personality),
          health(health), state(state), hours_since_last_conception(hours_since_last_conception),
          sleep_duration(sleep_duration), fetus(fetus), summary() {
    Cat::_next_id++;
    this->_force = Vec2::zero();
    this->_health = this->health;
    this->_step_finalized = true;
}

void Cat::free() {
    for (const auto &cat: Cat::all) {
        delete cat;
    }
}

void Cat::update_state_hours(size_t hours) {
    this->summary.update_state_hours(this->state, hours);
//    if (this->is_pregnant()) {
//        this->fetus->update_state_hours(hours);
//    }
}

void Cat::damage_health(double amount) {
    Logger::log() << "[Alert] " << this << " got damaged " << amount;
    auto final_amount = std::max(0.0, this->_health - amount);
    this->summary.update_lost_health(this->_health - final_amount);
    this->_health = final_amount;
}

void Cat::age_up(size_t hours) {
    auto years = static_cast<double>(hours) / (365 * 24);
    this->age += years;
    this->summary.update_aged(years);
    this->damage_health(1);
    if (this->is_sleeping()) {
        this->sleep_duration += hours;
    }
    if (this->hours_since_last_conception >= 0) {
        this->hours_since_last_conception += hours;
    }
}

void Cat::add_force(const Vec2 &force) {
    this->_force = this->_force + force;
}

void Cat::start_step() {
    if (this->_step_finalized) {
        this->_step_finalized = false;
    } else {
        throw std::exception();
    }
}

void Cat::finalize_step() {
    this->_force = Vec2::zero();
    this->health = this->_health;
    this->age_up();
    this->_step_finalized = true;
}

double Cat::mutual_attraction(const Cat *other_cat, double temperature) const {
    if (this->is_sleeping()) {
        return 0.0;
    }
    if (this->gender != other_cat->gender) {
        if (this->is_sexually_active() && other_cat->is_sexually_active()) {
            return temperature > 28 ? 0.9 : 0.7;
        } else {
            return 0.75 * this->_mutual_attraction_factor(other_cat);
        }
    } else {
        return this->_mutual_attraction_factor(other_cat);
    }
}

double Cat::_mutual_attraction_factor(const Cat *other_cat) const {
    if (this->personality == other_cat->personality) {
        return 0.7;
    } else {
        return this->_dominance_factor(other_cat);
    }
}

double Cat::_dominance_factor(const Cat *other_cat) const {
    return this->_strength() - other_cat->_strength();
}

double Cat::_strength() const {
    return this->health / 50.0;
}

double Cat::food_attraction() const {
    if (this->is_sleeping()) {
        return 0.0;
    }
    return -std::log(this->health / this->max_health());
}

double Cat::bed_attraction() const {
    if (this->is_sleeping()) {
        return 0.0;
    }
    if (this->age < 2. / 12) {
        return 0.5;
    }
    if (this->health > 95) {
        return 0.3;
    }
    return 0.1;
}

double Cat::box_attraction() const {
    return this->bed_attraction();
}

double Cat::trace_attraction(double x_trace, double y_trace) const {
    if (this->is_sleeping()) {
        return 0.0;
    }
    auto same_personality_trace = this->personality == Personality::X ? x_trace : y_trace;
    auto opposite_personality_trace = this->personality == Personality::X ? y_trace : x_trace;
    auto effective_trace = same_personality_trace - opposite_personality_trace;
    return effective_trace * Config::trace_attraction_factor;
}

Vec2 Cat::random_force() const {
    if (this->is_sleeping()) {
        return Vec2::zero();
    }
    return {Random::next<double>(-1, 1), Random::next<double>(-1, 1)};
}

void Cat::move(const Vec2 &target_position, double health_damage) {
    Logger::log() << "[Action] " << this << " is moving to " << target_position;
    auto distance = (target_position - this->position).norm();
    this->position = target_position;
    this->damage_health(health_damage);
    this->summary.update_moved(distance);
}

void Cat::interact(Cat *other_cat, double temperature) {
    if (this->cat_id == other_cat->cat_id) {
        return;
    }
    if (this->is_sleeping() || other_cat->is_sleeping()) {
        return;
    }
    if (this->gender != other_cat->gender) {
        if (this->is_sexually_active() && other_cat->is_sexually_active()) {
            auto reproduction_probability = temperature > 28 ? 0.9 : 0.7;
            if (auto random = Random::next<double>(0, 1); random < reproduction_probability && this->health > 25 &&
                                                          other_cat->health > 25) {
                auto pair = choose_female_cat(this, other_cat);
                auto &female_cat = pair.first;
                auto &male_cat = pair.second;
                this->damage_health(20);
                other_cat->damage_health(20);
                female_cat->conceive(male_cat);
                return;
            }
        }
    }
    if (this->personality != other_cat->personality) {
        auto dominance = this->_dominance_factor(other_cat);
        auto attacking_probability = std::max(0.0, dominance);
        if (auto random = Random::next<double>(0, 1); random < attacking_probability) {
            auto power = dominance * 10;
            this->attack(other_cat, power);
            return;
        }
    }
}

void Cat::attack(Cat *other_cat, double power) {
    Logger::log() << "[Action] " << this << " is attacking " << other_cat << " with " << power << " power";
    other_cat->damage_health(power);
    this->damage_health(power / 5);
    other_cat->summary.update_got_attacked(power);
    this->summary.update_attacked(power);
}

void Cat::conceive(Cat *partner_cat) {
    Logger::log() << "[Action] " << this << " got conceived by " << partner_cat;
    this->hours_since_last_conception = 0;
    this->summary.update_conceived();
    Personality fetus_personality;
    if (this->personality == partner_cat->personality) {
        fetus_personality = this->personality;
    } else {
        fetus_personality = Enum<Personality>::random();
    }
    this->fetus = Cat::create(
            this->position,
            0,
            Enum<Gender>::random(),
            fetus_personality,
            ::max_health(0),
            State::fetus
    );
}

Cat *Cat::deliver() {
    if (this->fetus == nullptr) {
        return nullptr;
    }
    auto cat_baby = this->fetus;
    this->fetus = nullptr;
    cat_baby->position = this->position;
    cat_baby->state = State::active;
    this->summary.update_delivered();
    Logger::log() << "[Action] " << this << " delivered " << cat_baby;
    return cat_baby;
}

void Cat::consume_food(double amount) {
    Logger::log() << "[Action] " << this << " is consuming " << amount << " foods";
    auto final_amount = std::min(this->max_health(), this->_health + amount);
    this->summary.update_consumed_food(final_amount - this->_health);
    this->_health = final_amount;
}

void Cat::wake_up() {
    Logger::log() << "[Action] " << this << " is wake-up after " << this->sleep_duration << " hours";
    this->state = State::active;
    this->sleep_duration = 0;
}

void Cat::sleep() {
    Logger::log() << "[Action] " << this << " is starting to sleep";
    this->state = State::sleeping;
    this->sleep_duration = 0;
}

void Cat::die() {
    Logger::log() << "[Action] RIP " << this << " :(";
    this->state = State::dead;
}

bool Cat::is_pregnant() const {
    return this->gender == Gender::female && this->fetus != nullptr;
}

bool Cat::is_sexually_active() const {
    return this->age > 4 / 12.0 && !this->is_pregnant();
}

bool Cat::is_sleeping() const {
    return this->state == State::sleeping;
}

bool Cat::is_alive() const {
    return this->state == State::active || this->state == State::sleeping;
}

bool Cat::should_die() const {
    return this->health <= 0 || this->age >= Config::max_life_span;
}

Vec2 Cat::get_force() const {
    return this->_force;
}

double Cat::max_health() const {
    return ::max_health(this->age);
}

std::ostream &operator<<(std::ostream &os, Cat *cat) {
    os << "Cat{";
    os << "cat_id=" << cat->cat_id << ", ";
    os << "position=" << cat->position << ", ";
    os << "age=" << cat->age << ", ";
    os << "health=" << cat->health << ", ";
    os << "gender=" << cat->gender << ", ";
    os << "personality=" << cat->personality << ", ";
    os << "state=" << cat->state;
    os << "}";
    return os;
}
