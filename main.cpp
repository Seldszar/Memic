#define MINIAUDIO_IMPLEMENTATION
#define NOMINMAX

#include <expected>
#include <filesystem>
#include <print>
#include <random>
#include <ranges>
#include <thread>
#include <unordered_map>

#include "miniaudio.h"

using namespace std::chrono_literals;

struct Args {
	std::unordered_map<std::string, std::string> m;

	Args(int argc, char** argv) {
		for (int i = 1; i < argc; ++i) {
			std::string_view a = argv[i];

			if (!a.starts_with("-"))
				continue;

			a.remove_prefix(a.starts_with("--") ? 2 : 1);

			std::string_view v = "true";

			if (auto p = a.find('='); p != std::string_view::npos) {
				v = a.substr(p + 1); a = a.substr(0, p);
			}
			else if (i + 1 < argc && !std::string_view(argv[i + 1]).starts_with("-")) {
				v = argv[++i];
			}

			m[std::string(a)] = v;
		}
	}

	template <typename T> T get(std::string key, T implicit = {}) const {
		auto it = m.find(key);

		if (it == m.end())
			return implicit;

		const auto& s = it->second;

		if constexpr (std::same_as<T, std::string>)
			return s;

		if constexpr (std::same_as<T, bool>)
			return s == "true" || s == "1";

		if constexpr (std::integral<T>) {
			T r{}; std::from_chars(s.data(), s.data() + s.size(), r); return r;
		}

		if constexpr (std::floating_point<T>)
			return static_cast<T>(std::stod(s));

		return implicit;
	}
};

struct Clip {
	const std::filesystem::path& path;

	const double& pan;
	const double& pitch;
	const double& volume;
};

static std::expected<void, std::string> play_clip(ma_engine& engine, Clip& clip) {
	ma_sound sound;

	if (ma_sound_init_from_file(&engine, clip.path.string().c_str(), 0, nullptr, nullptr, &sound))
		return std::unexpected("Failed to create sound");

	ma_sound_set_pan(&sound, clip.pan);
	ma_sound_set_pitch(&sound, clip.pitch);
	ma_sound_set_volume(&sound, clip.volume);

	ma_sound_start(&sound);

	while (ma_sound_is_playing(&sound))
		std::this_thread::sleep_for(20ms);

	ma_sound_uninit(&sound);

	return {};
}

int main(int argc, char** argv) {
	Args args(argc, argv);

	std::println(R"(8b   d8                 w     )");
	std::println(R"(8YbmdP8 .d88b 8d8b.d8b. w .d8b)");
	std::println(R"(8  "  8 8.dP' 8P Y8P Y8 8 8   )");
	std::println(R"(8     8 `Y88P 8   8   8 8 `Y8P)");
	std::println();

	auto input_path = args.get("input", "sounds");
	auto random_seed = args.get("seed", time(nullptr));

	auto delay_lo = args.get("delay[lo]", 60);
	auto delay_hi = args.get("delay[hi]", 120);

	auto pan_lo = args.get("pan[lo]", 0.);
	auto pan_hi = args.get("pan[hi]", 0.);

	auto pitch_lo = args.get("pitch[lo]", .5);
	auto pitch_hi = args.get("pitch[hi]", 2.);

	auto volume_lo = args.get("volume[lo]", 1.);
	auto volume_hi = args.get("volume[hi]", 1.);

	std::println("Input Path: \"{}\"", input_path);
	std::println("Random Seed: {}", random_seed);
	std::println("Delay Range: {}-{} second(s)", delay_lo, delay_hi);
	std::println("Pan Range: {}-{}", pan_lo, pan_hi);
	std::println("Pitch Range: {}-{}", pitch_lo, pitch_hi);
	std::println("Volume Range: {}-{}", volume_lo, volume_hi);
	std::println();

	auto clip_paths = std::filesystem::directory_iterator(input_path)
		| std::views::filter([](const auto& e) { return e.is_regular_file(); })
		| std::views::transform([](const auto& e) { return e.path(); })
		| std::ranges::to<std::vector<std::filesystem::path>>();

	if (clip_paths.empty()) {
		std::println(stderr, "No sounds available. Exiting...");
		return 1;
	}

	std::println("Found {} files:", clip_paths.size());

	for (const auto& c : clip_paths)
		std::println(" - {}", c.string());

	std::println();

	std::mt19937 rng(random_seed);

	std::uniform_int_distribution random_delay(delay_lo, delay_hi);
	std::uniform_real_distribution random_pan(pan_lo, pan_hi);
	std::uniform_real_distribution random_pitch(pitch_lo, pitch_hi);
	std::uniform_real_distribution random_volume(volume_lo, volume_hi);
	std::uniform_int_distribution<size_t> random_clip(0, clip_paths.size() - 1);

	ma_engine engine;

	if (ma_engine_init(nullptr, &engine)) {
		std::println(stderr, "Failed to initialize audio engine");
		return 1;
	}

	std::println("Starting loop...");
	std::println();

	while (true) {
		std::this_thread::sleep_for(std::chrono::duration<double>(random_delay(rng)));

		Clip clip{
			clip_paths.at(random_clip(rng)),

			random_pan(rng),
			random_pitch(rng),
			random_volume(rng),
		};

		std::println("Playing \"{}\"... (pan={} pitch={}, volume={})", clip.path.string(), clip.pan, clip.pitch, clip.volume);

		if (auto r = play_clip(engine, clip); !r)
			std::println(stderr, "Error while playing sound: {}", r.error());
	}

	return 0;
}
