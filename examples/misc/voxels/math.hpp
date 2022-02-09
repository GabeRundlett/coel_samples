#include <concepts>
#include <cstdint>
#include <cstddef>
#include <array>
#include <limits>
#include <cmath>
#include <type_traits>

template <typename T>
concept vec_like = requires(T v, size_t i) {
    v[i];
};

template <typename T>
concept scalar = std::integral<T> || std::floating_point<T>;

template <vec_like T>
using vec_value_t = std::decay_t<decltype(std::declval<T>()[0])>;

template <vec_like T_>
consteval size_t vec_size() {
    using T = std::remove_reference_t<T_>;
    using V = vec_value_t<T_>;
    return sizeof(T) / sizeof(V);
}

template <vec_like T1, vec_like... Ts>
consteval auto vec_validate_size() {
    constexpr auto VSIZE = vec_size<T1>();
    static_assert(((VSIZE == vec_size<Ts>()) && ...));
    return VSIZE;
}

constexpr auto vec_operator(vec_like auto &&v1, vec_like auto &&v2,
                            auto op_init_fn, auto op_fn) {
    constexpr auto VSIZE = vec_validate_size<decltype(v1), decltype(v2)>();
    auto result = op_init_fn(v1, v2);
    for (size_t i = 0; i < VSIZE; ++i)
        op_fn(result, i, v1[i], v2[i]);
    return result;
}

constexpr auto vec_operator(vec_like auto &&v1, scalar auto fac,
                            auto op_init_fn, auto op_fn) {
    constexpr auto VSIZE = vec_size<decltype(v1)>();
    auto result = op_init_fn(v1, fac);
    for (size_t i = 0; i < VSIZE; ++i)
        op_fn(result, i, v1[i], fac);
    return result;
}

constexpr auto operator+(vec_like auto &&v1, vec_like auto &&v2) {
    return vec_operator(
        v1, v2,
        [](auto v1, auto) { return v1; },
        [](auto &result, auto i, auto, auto v2_v) { result[i] += v2_v; });
}
constexpr auto &operator+=(vec_like auto &v1, vec_like auto &&v2) { return v1 = v1 + v2; }

constexpr auto operator-(vec_like auto &&v1, vec_like auto &&v2) {
    return vec_operator(
        v1, v2,
        [](auto v1, auto) { return v1; },
        [](auto &result, auto i, auto, auto v2_v) { result[i] -= v2_v; });
}
constexpr auto &operator-=(vec_like auto &v1, vec_like auto &&v2) { return v1 = v1 - v2; }

// constexpr auto operator*(vec_like auto &&v1, vec_like auto &&v2) {
//     return vec_operator(
//         v1, v2,
//         [](auto v1, auto) { return v1; },
//         [](auto &result, auto i, auto, auto v2_v) { result[i] *= v2_v; });
// }
// constexpr auto &operator*=(vec_like auto &v1, vec_like auto &&v2) { return v1 = v1 * v2; }

constexpr auto operator*(vec_like auto &&v1, scalar auto fac) {
    return vec_operator(
        v1, fac,
        [](auto v1, auto) { return v1; },
        [](auto &result, auto i, auto, auto fac_v) { result[i] *= fac_v; });
}
constexpr auto &operator*=(vec_like auto &v1, scalar auto fac) { return v1 = v1 * fac; }

constexpr auto operator/(vec_like auto &&v1, vec_like auto &&v2) {
    return vec_operator(
        v1, v2,
        [](auto v1, auto) { return v1; },
        [](auto &result, auto i, auto, auto v2_v) { result[i] /= v2_v; });
}
constexpr auto &operator/=(vec_like auto &v1, vec_like auto &&v2) { return v1 = v1 / v2; }

constexpr auto operator/(vec_like auto &&v1, scalar auto fac) {
    return vec_operator(
        v1, fac,
        [](auto v, auto) { return v; },
        [](auto &result, auto i, auto, auto fac_v) { result[i] /= fac_v; });
}
constexpr auto &operator/=(vec_like auto &v1, scalar auto fac) { return v1 = v1 / fac; }

constexpr auto dot(vec_like auto &&v1, vec_like auto &&v2) {
    return vec_operator(
        v1, v2,
        [](auto, auto) { return vec_value_t<decltype(v1)>(0); },
        [](auto &result, auto, auto v1_v, auto v2_v) { result += v1_v * v2_v; });
}

namespace detail {
    constexpr float sqrtNewtonRaphson(float x, float curr, float prev) {
        return curr == prev ? curr : sqrtNewtonRaphson(x, 0.5f * (curr + x / curr), curr);
    }
} // namespace detail

template <std::floating_point F>
constexpr auto square_root(F x) {
    if (std::is_constant_evaluated()) {
        return x >= 0 && x < std::numeric_limits<decltype(x)>::infinity()
                   ? detail::sqrtNewtonRaphson(x, x, F(0))
                   : std::numeric_limits<F>::quiet_NaN();
    }
    return F(sqrt(x));
}

constexpr auto mag(vec_like auto &&v) {
    return square_root(dot(v, v));
}

constexpr auto normalize(vec_like auto &&v) {
    return v / mag(v);
}

#if 0

template <typename ValueT, size_t N>
struct RaycastResult {
    std::array<int, N> tile_index{};
    uint32_t total_steps;
    bool hit_surface;
    uint32_t hit_edge_i;
    std::array<f32vec2, 32> points;
};

#if 0
constexpr auto raycast_internal(vec_like auto && ray_origin, vec_like auto && ray_dir, auto in_bounds, auto is_tile_blocking) {
    constexpr auto N = vec_validate_size<
        decltype(ray_origin),
        decltype(ray_dir)
    >();
    using ValueT = vec_value_t<decltype(ray_origin)>;
    RaycastResult<ValueT, N> result{};
    auto abs = [](auto v) {
        if (v < 0) return -v;
        return v;
    };
    std::array<std::array<ValueT, N - 1>, N> delta_dist;
    std::array<std::array<ValueT, N - 1>, N> to_side_dist;
    std::array<int, N> ray_step;
    for (size_t i = 0; i < N; ++i)
        result.tile_index[i] = static_cast<int>(ray_origin[i]);
    for (auto i = 0ul; i < N; ++i) {
        for (auto j = 0ul; j < N - 1; ++j) {
            const auto n = j + (i <= j);
            delta_dist[i][j] = ray_dir[n] == 0 ? 0 : ray_dir[i] == 0 ? 1 : abs(1 / ray_dir[i]);
            if (ray_dir[i] < ValueT(0)) {
                ray_step[i] = -1;
                to_side_dist[i][j] = ray_origin[i] - ray_dir[i];
            } else {
                ray_step[i] = 1;
                to_side_dist[i][j] = ray_origin[i] + ValueT(1) - result.tile_index[i];
            }
            to_side_dist[i][j] *= delta_dist[i][j];
        }
    }
    for (result.total_steps = 0; result.total_steps < 1'000; ++result.total_steps) {
        if (!in_bounds(result.tile_index))
            break;
        auto &p = result.points[result.total_steps];
        p = {};
        for (auto i = 0ul; i < N; ++i)
            p[i] = (f32)result.tile_index[i] + 0.5f;
        if (is_tile_blocking(result.tile_index)) {
            result.hit_surface = true;
            break;
        }
        size_t current_edge = 0;
        for (size_t i = 1; i < N; ++i) {
            if (to_side_dist[current_edge][i - (current_edge <= i)] >
                to_side_dist[i][current_edge - (i <= current_edge)])
                current_edge = i;
        }
        for (size_t i = 0; i < N - 1; ++i) {
            const auto n = i + (current_edge < i);
            to_side_dist[current_edge][n] += delta_dist[current_edge][n];
        }
        result.tile_index[current_edge] += ray_step[current_edge];
    }
    return result;
}
#else
constexpr auto raycast_internal(f32vec2 ray_origin, f32vec2 ray_dir, auto in_bounds, auto is_tile_blocking) {
    RaycastResult<float, 2> result;
    result.tile_index = {(i32)ray_origin[0] - (ray_origin[0] < 0 ? 1 : 0), (i32)ray_origin[1] - (ray_origin[1] < 0 ? 1 : 0)};
    result.total_steps = 0;
    result.hit_surface = false;
    result.hit_edge_i = false;
    f32vec2 delta_dist{
        ray_dir[1] == 0 ? 0 : (ray_dir[0] == 0 ? 1 : std::abs(1.0f / ray_dir[0])),
        ray_dir[0] == 0 ? 0 : (ray_dir[1] == 0 ? 1 : std::abs(1.0f / ray_dir[1])),
    };
    f32vec2 to_side_dist;
    i32vec2 ray_step;
    if (ray_dir[0] < 0) {
        ray_step[0] = -1, to_side_dist[0] = (ray_origin[0] - result.tile_index[0]) * delta_dist[0];
    } else {
        ray_step[0] = 1, to_side_dist[0] = (result.tile_index[0] + 1.0f - ray_origin[0]) * delta_dist[0];
    }
    if (ray_dir[1] < 0) {
        ray_step[1] = -1, to_side_dist[1] = (ray_origin[1] - result.tile_index[1]) * delta_dist[1];
    } else {
        ray_step[1] = 1, to_side_dist[1] = (result.tile_index[1] + 1.0f - ray_origin[1]) * delta_dist[1];
    }
    while (result.total_steps < 32) {
        if (!in_bounds(result.tile_index))
            break;
        result.points[result.total_steps] = f32vec2{(f32)result.tile_index[0] + 0.5f, (f32)result.tile_index[1] + 0.5f};
        if (is_tile_blocking(result.tile_index)) {
            result.hit_surface = true;
            break;
        }
        if (to_side_dist[0] < to_side_dist[1]) {
            to_side_dist[0] += delta_dist[0];
            result.tile_index[0] += ray_step[0];
            result.hit_edge_i = false;
        } else {
            to_side_dist[1] += delta_dist[1];
            result.tile_index[1] += ray_step[1];
            result.hit_edge_i = true;
        }
        ++result.total_steps;
    }
    return result;
}
#endif

constexpr auto raycast(f32vec2 ray_origin, f32vec2 ray_dir, f32vec2 bound_min, f32vec2 bound_max, auto is_tile_blocking) {
    auto in_bounds = [&](auto tile_i) {
        return tile_i[0] >= bound_min[0] &&
               tile_i[0] < bound_max[0] &&
               tile_i[1] >= bound_min[1] &&
               tile_i[1] < bound_max[1];
    };
    bool is_vertical = false;
    if (!in_bounds(ray_origin)) {
        float slope_xy = ray_dir[0] / ray_dir[1];
        float slope_yx = ray_dir[1] / ray_dir[0];
        auto nudge = ray_dir * 0.0001f;
        bool hit = false;
        if (!hit && ray_origin[1] < bound_min[1]) {
            auto p = bound_min;
            p = f32vec2{ray_origin[0] + (p[1] - ray_origin[1]) * slope_xy, p[1]} + nudge;
            if (in_bounds(p))
                hit = true, ray_origin = p, is_vertical = true;
        }
        if (!hit && ray_origin[0] < bound_min[0]) {
            auto p = bound_min;
            p = f32vec2{p[0], ray_origin[1] + (p[0] - ray_origin[0]) * slope_yx} + nudge;
            if (in_bounds(p))
                hit = true, ray_origin = p, is_vertical = false;
        }
        if (!hit && ray_origin[1] >= bound_max[1]) {
            auto p = bound_max;
            p = f32vec2{ray_origin[0] + (p[1] - ray_origin[1]) * slope_xy, p[1]} + nudge;
            if (in_bounds(p))
                hit = true, ray_origin = p, is_vertical = true;
        }
        if (!hit && ray_origin[0] >= bound_max[0]) {
            auto p = bound_max;
            p = f32vec2{p[0], ray_origin[1] + (p[0] - ray_origin[0]) * slope_yx} + nudge;
            if (in_bounds(p))
                hit = true, ray_origin = p, is_vertical = false;
        }
        if (!hit)
            return RaycastResult<float, 2>{};
    }
    auto r = raycast_internal(ray_origin, ray_dir, in_bounds, is_tile_blocking);
    if (r.total_steps == 0)
        r.hit_edge_i = is_vertical;
    return r;
}
#else
template <typename T, size_t N>
struct RaycastResult {
    std::array<int32_t, N> tile_index{};
    size_t total_steps, hit_edge_i;
    bool hit_surface;
    std::array<std::array<T, N>, 32> points;
};

constexpr auto raycast(vec_like auto &&ray_o, vec_like auto &&ray_d, vec_like auto &&bound_min, vec_like auto &&bound_max, auto is_tile_blocking) {
    constexpr auto N = vec_validate_size<decltype(ray_o), decltype(ray_d), decltype(bound_min), decltype(bound_max)>();
    using T = vec_value_t<decltype(ray_o)>;
    auto in_bounds = [&bound_min, &bound_max](vec_like auto p) {
        for (size_t i = 0; i < N; ++i)
            if (p[i] < bound_min[i] || p[i] > bound_max[i])
                return false;
        return true;
    };
    RaycastResult<T, N> result{};
    auto ray_origin = ray_o;
    // Pass 1: correct ray_o to be within the bounds
    bool hit_axis_i = false;
    if (!in_bounds(ray_o)) {
        std::array<std::array<T, N - 1>, N> slopes{};
        for (size_t axis_i = 0; axis_i < N; ++axis_i) {
            for (size_t rel_axis_store_i = 0; rel_axis_store_i < N - 1; ++rel_axis_store_i) {
                auto rel_axis_i = rel_axis_store_i + static_cast<size_t>(rel_axis_store_i >= axis_i);
                slopes[axis_i][rel_axis_store_i] = ray_d[axis_i] / ray_d[rel_axis_i];
            }
        }
        auto nudge = ray_d * 0.0001f;
        bool hit = false;
        for (size_t axis_i = 0; axis_i < N; ++axis_i) {
            // under
            if (!hit && ray_o[axis_i] < bound_min[axis_i]) {
                auto p = bound_min;
                for (size_t rel_axis_store_i = 0; rel_axis_store_i < N - 1; ++rel_axis_store_i) {
                    auto rel_axis_i = rel_axis_store_i + static_cast<size_t>(rel_axis_store_i >= axis_i);
                    p[rel_axis_i] = ray_o[rel_axis_i] + (p[axis_i] - ray_o[axis_i]) * slopes[rel_axis_i][rel_axis_store_i];
                }
                p += nudge;
                if (in_bounds(p))
                    hit = true, ray_origin = p, hit_axis_i = axis_i;
            }
            // over
            if (!hit && ray_o[axis_i] >= bound_max[axis_i]) {
                auto p = bound_max;
                for (size_t rel_axis_store_i = 0; rel_axis_store_i < N - 1; ++rel_axis_store_i) {
                    auto rel_axis_i = rel_axis_store_i + static_cast<size_t>(rel_axis_store_i >= axis_i);
                    p[rel_axis_i] = ray_o[rel_axis_i] + (p[axis_i] - ray_o[axis_i]) * slopes[rel_axis_i][rel_axis_store_i];
                }
                p += nudge;
                if (in_bounds(p))
                    hit = true, ray_origin = p, hit_axis_i = axis_i;
            }
        }
        if (!hit) return RaycastResult<T, N>{};
    }
    // Pass 2: set up the raycast state
    for (size_t i = 0; i < N; ++i)
        result.tile_index[i] = static_cast<int32_t>(ray_origin[i]) - (ray_origin[i] < 0 ? 1 : 0);
    auto abs = [](auto x) {
        if (x < 0) return -x;
        return x;
    };
    std::array<std::array<T, N - 1>, N> delta_dists{};
    std::array<std::array<T, N - 1>, N> to_side_dists{};
    std::array<int32_t, N> ray_step{};
    for (size_t axis_i = 0; axis_i < N; ++axis_i) {
        for (size_t rel_axis_store_i = 0; rel_axis_store_i < N - 1; ++rel_axis_store_i) {
            auto rel_axis_i = rel_axis_store_i + static_cast<size_t>(rel_axis_store_i >= axis_i);
            delta_dists[axis_i][rel_axis_store_i] = ray_d[rel_axis_i] == 0 ? 0 : (ray_d[axis_i] == 0 ? 1 : abs(1 / ray_d[axis_i]));
        }
    }
    for (size_t axis_i = 0; axis_i < N; ++axis_i) {
        for (size_t rel_axis_store_i = 0; rel_axis_store_i < N - 1; ++rel_axis_store_i) {
            // auto rel_axis_i = rel_axis_store_i + static_cast<size_t>(rel_axis_store_i >= axis_i);
            if (ray_d[axis_i] < 0) {
                ray_step[axis_i] = -1;
                to_side_dists[axis_i][rel_axis_store_i] = (ray_origin[axis_i] - result.tile_index[axis_i]) * delta_dists[axis_i][rel_axis_store_i];
            } else {
                ray_step[axis_i] = 1;
                to_side_dists[axis_i][rel_axis_store_i] = (result.tile_index[axis_i] + 1 - ray_origin[axis_i]) * delta_dists[axis_i][rel_axis_store_i];
            }
        }
    }
    size_t max_steps = 0;
    for (size_t axis_i = 0; axis_i < N; ++axis_i)
        max_steps += static_cast<size_t>(bound_max[axis_i] - bound_min[axis_i]);
    // Pass 3: Run the DDA algorithm
    for (result.total_steps = 0; result.total_steps < max_steps;
         ++result.total_steps) {
        if (!in_bounds(result.tile_index)) break;
        
        for (size_t i = 0; i < N; ++i)
            result.points[result.total_steps][i] = static_cast<T>(result.tile_index[i]) + static_cast<T>(0.5);
        if (is_tile_blocking(result.tile_index)) {
            result.hit_surface = true;
            break;
        }

        size_t i = 0;
        for (size_t axis_i = 0; axis_i < N - 1; ++axis_i) {
            if (to_side_dists[i][0] >= to_side_dists[axis_i + 1][i])
                i = axis_i + 1;
        }
        result.tile_index[i] += ray_step[i];
        for (size_t axis_i = 0; axis_i < N - 1; ++axis_i)
            to_side_dists[i][axis_i] += delta_dists[i][axis_i];
        result.hit_edge_i = i;
    }
    if (result.total_steps == 0)
        result.hit_edge_i = hit_axis_i;
    return result;
}

// constexpr std::array<size_t, 2> TILE_DIM_2D{4, 4};
// constexpr std::array<uint32_t, TILE_DIM_2D[0] * TILE_DIM_2D[1]> tiles_2d{
//     1, 1, 1, 1,
//     1, 0, 0, 1,
//     1, 0, 0, 1,
//     1, 1, 1, 1,
// };
// using f32vec2 = std::array<float, 2>;
// constexpr f32vec2 ray2d_o{-1, 2}, ray2d_d = normalize(f32vec2{1, 1});
// constexpr auto r_2d =
//     raycast(ray2d_o, ray2d_d, f32vec2{0, 0}, f32vec2{4, 4}, [](vec_like auto p) {
//         return tiles_2d[static_cast<size_t>(p[0] + p[1] * TILE_DIM_2D[0])] != 0;
//     });
// auto r2d = r_2d;

// constexpr std::array<size_t, 3> TILE_DIM_3D{4, 4, 4};
// constexpr std::array<uint32_t, TILE_DIM_3D[0] * TILE_DIM_3D[1] * TILE_DIM_3D[2]> tiles_3d{
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 0, 0, 1,
//     1, 0, 0, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 0, 0, 1,
//     1, 0, 0, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
//     1, 1, 1, 1,
// };
// using f32vec3 = std::array<float, 3>;
// constexpr f32vec3 ray3d_o{-1, 2, 0}, ray3d_d = normalize(f32vec3{1, 1, 1});
// constexpr auto r_3d =
//     raycast(ray3d_o, ray3d_d, f32vec3{0, 0, 0}, f32vec3{4, 4, 4}, [](vec_like auto p) {
//         return tiles_3d[static_cast<size_t>(p[0] + p[1] * TILE_DIM_3D[0] + p[2] * TILE_DIM_3D[0] * TILE_DIM_3D[1])] != 0;
//     });
// auto r3d = r_3d;
#endif

struct SurfaceDetails {
    f32vec2 pos, nrm;
};
constexpr auto get_surface_details(f32vec2 ray_origin, f32vec2 ray_dir, const RaycastResult<float, 2> &result) {
    SurfaceDetails surface;
    surface.pos = {(f32)result.tile_index[0], (f32)result.tile_index[1]};
    float slope_xy = ray_dir[0] / ray_dir[1];
    float slope_yx = ray_dir[1] / ray_dir[0];
    if (result.hit_edge_i) {
        if (ray_dir[1] < 0) {
            surface.pos[1] += 1;
            surface.nrm = f32vec2{0.0f, 1.0f};
        } else {
            surface.nrm = f32vec2{0.0f, -1.0f};
        }
        surface.pos = f32vec2{ray_origin[0] + (surface.pos[1] - ray_origin[1]) * slope_xy, surface.pos[1]};
    } else {
        if (ray_dir[0] < 0) {
            surface.pos[0] += 1;
            surface.nrm = f32vec2{1.0f, 0.0f};
        } else {
            surface.nrm = f32vec2{-1.0f, 0.0f};
        }
        surface.pos = f32vec2{surface.pos[0], ray_origin[1] + (surface.pos[0] - ray_origin[0]) * slope_yx};
    }

    return surface;
}
