#include <gcs/constraints/constraints_test_utils.hh>
#include <gcs/constraints/not_equals.hh>
#include <gcs/problem.hh>
#include <gcs/solve.hh>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using std::cerr;
using std::flush;
using std::function;
using std::index_sequence;
using std::make_optional;
using std::mt19937;
using std::nullopt;
using std::pair;
using std::random_device;
using std::set;
using std::string;
using std::to_string;
using std::tuple;
using std::uniform_int_distribution;
using std::vector;

using fmt::print;
using fmt::println;

using namespace gcs;
using namespace gcs::test_innards;

auto run_not_equals_test(bool proofs, pair<int, int> v1_range, pair<int, int> v2_range, const function<auto(int, int)->bool> & is_satisfying) -> void
{
    print(cerr, "not equals {} {} {}", v1_range, v2_range, proofs ? " with proofs:" : ":");
    cerr << flush;

    set<pair<int, int>> expected, actual;
    build_expected(expected, is_satisfying, v1_range, v2_range);
    println(cerr, " expecting {} solutions", expected.size());

    Problem p;
    auto v1 = p.create_integer_variable(Integer(v1_range.first), Integer(v1_range.second));
    auto v2 = p.create_integer_variable(Integer(v2_range.first), Integer(v2_range.second));
    p.post(NotEquals{v1, v2});

    auto proof_name = proofs ? make_optional("not_equals_test") : nullopt;
    solve_for_tests_checking_gac(p, proof_name, expected, actual, tuple{v1, v2});

    check_results(proof_name, expected, actual);
}

auto main(int, char *[]) -> int
{
    vector<pair<pair<int, int>, pair<int, int>>> data = {
        {{2, 5}, {1, 6}},
        {{1, 6}, {2, 5}},
        {{1, 3}, {1, 3}},
        {{1, 5}, {6, 8}},
        {{1, 1}, {2, 4}},
        {{-2, -2}, {-2, -1}}};

    random_device rand_dev;
    mt19937 rand(rand_dev());
    for (int x = 0; x < 10; ++x)
        generate_random_data(rand, data, random_bounds(-10, 10, 5, 15), random_bounds(-10, 10, 5, 15));

    for (auto & [r1, r2] : data)
        run_not_equals_test(false, r1, r2, [](int a, int b) { return a != b; });

    if (can_run_veripb())
        for (auto & [r1, r2] : data)
            run_not_equals_test(true, r1, r2, [](int a, int b) { return a != b; });

    return EXIT_SUCCESS;
}
