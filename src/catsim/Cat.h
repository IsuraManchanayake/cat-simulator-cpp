//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_CAT_H
#define CAT_SIMULATOR_CPP_CAT_H

#include <map>
#include <ostream>
#include <vector>

#include "Enums.h"
#include "Logger.h"
#include "Vec2.h"

struct CatSummary {
    std::map<State, size_t> state_hours;
    double attacked;
    double got_attacked;
    size_t conceived;
    size_t delivered;
    double lost_health;
    double consumed_food;
    double moved;
    double aged;

    CatSummary();

    void update_moved(double distance);
    void update_conceived();
    void update_attacked(double amount);
    void update_got_attacked(double amount);
    void update_delivered();
    void update_consumed_food(double amount);
    void update_lost_health(double amount);
    void update_aged(double years);
    void update_state_hours(State state, size_t hours);
};

std::ostream &operator<<(std::ostream &os, const CatSummary &cat);

struct Cat {
    static int _next_id;

    const int cat_id;
    Vec2 position;
    double age;
    const Gender gender;
    const Personality personality;
    double health;
    State state;
    size_t hours_since_last_conception;
    size_t sleep_duration;
    Cat *fetus;

    CatSummary summary;

    Vec2 _force;
    double _health;
    bool _step_finalized;

    static std::vector<Cat *> all;

private:
    Cat(const Vec2 &position, double age, Gender gender, Personality personality, double health,
        State state, size_t sleep_duration = 0, Cat *fetus = nullptr, size_t hours_since_last_conception = -1,
        int cat_id = -1);

public:
    template<typename ...Args>
    static Cat *create(Args... args);
    static void free();

    void update_state_hours(size_t hours=1);
    void damage_health(double amount);
    void age_up(size_t hours=1);
    void add_force(const Vec2 &force);
    void start_step();
    void finalize_step();

    [[nodiscard]] double mutual_attraction(const Cat *other_cat, double temperature) const;
    [[nodiscard]] double _mutual_attraction_factor(const Cat *other_cat) const;
    [[nodiscard]] double _dominance_factor(const Cat *other_cat) const;
    [[nodiscard]] double _strength() const;
    [[nodiscard]] double food_attraction() const;
    [[nodiscard]] double bed_attraction() const;
    [[nodiscard]] double box_attraction() const;
    [[nodiscard]] double trace_attraction(double x_trace, double y_trace) const;
    [[nodiscard]] Vec2 random_force() const;

    void move(const Vec2 &target_position, double health_damage);
    void interact(Cat* other_cat, double temperature);
    void attack(Cat *other_cat, double power);
    void conceive(Cat *partner_cat);
    Cat *deliver();
    void consume_food(double amount);
    void wake_up();
    void sleep();
    void die();

    [[nodiscard]] bool is_pregnant() const;
    [[nodiscard]] bool is_sexually_active() const;
    [[nodiscard]] bool is_sleeping() const;
    [[nodiscard]] bool is_alive() const;
    [[nodiscard]] bool should_die() const;
    [[nodiscard]] Vec2 get_force() const;
    [[nodiscard]] double max_health() const;
};

std::ostream &operator<<(std::ostream &os, Cat *cat);

template<typename ...Args>
Cat *Cat::create(Args... args) {
    const auto cat = new Cat(args...);
    Cat::all.push_back(cat);
    operator<<(Logger::log() << "[Create] Cat is created ", cat);
    return cat;
}

#endif //CAT_SIMULATOR_CPP_CAT_H
