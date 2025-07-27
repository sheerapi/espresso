#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace platform
{
	struct Version;
	struct SystemInfo;
	struct ApplicationState;
	struct PathInfo;
	struct RuntimeInfo;

	struct Version
	{
		uint16_t major = 0;
		uint16_t minor = 0;
		uint16_t patch = 0;
		std::string metadata; // e.g., "alpha", "beta", "rc1", or "alpha+data"

		[[nodiscard]] auto toString() const -> std::string
		{
			std::string result = std::to_string(major) + "." + std::to_string(minor) +
								 "." + std::to_string(patch);
			if (!metadata.empty())
			{
				result += "-" + metadata;
			}
			return result;
		}

		auto operator==(const Version& other) const -> bool
		{
			return major == other.major && minor == other.minor && patch == other.patch &&
				   metadata == other.metadata;
		}

		auto operator<(const Version& other) const -> bool
		{
			if (major != other.major)
			{
				return major < other.major;
			}
			if (minor != other.minor)
			{
				return minor < other.minor;
			}
			if (patch != other.patch)
			{
				return patch < other.patch;
			}
			return metadata < other.metadata;
		}
	};

	struct SystemInfo
	{
		std::string platform;		// "Windows", "Linux", "macOS", "iOS", "Android"
		std::string architecture;	// "x64", "x86", "ARM64", "ARM"
		std::string osVersion;		// operating system version
		std::string cpuBrand;		// cpu info
		uint32_t cpuCores = 0;		// well, cpu cores duh
		uint64_t totalMemoryMB = 0; // this is getting redundant
		uint64_t availableMemoryMB = 0;
	};

	// paths used by the engine
	struct PathInfo
	{
		std::string executablePath;
		std::string executableDir;
		std::string workingDirectory; // current working directory
		std::string logPath;		  // path to log files
		std::string cachePath;		  // path to cache directory
		std::string userDataPath;	  // path to user-specific data (saves, config)
		std::string tempPath;
	};

	enum class ExecutionMode : char
	{
		Game,	 // Running as a game
		Editor,	 // Running in editor mode
		Server,	 // Running as dedicated server
		Tool,	 // Running as command-line tool
		Test,	 // Running unit tests
		Profiler // Running with profiler attached
	};

	enum class BuildConfiguration : char
	{
		Debug,		 // Debug build with all debugging info
		Development, // Development build with some optimizations
		Shipping,	 // Final shipping build, fully optimized
		Test		 // Test build for automated testing
	};

	struct ApplicationInfo
	{
	public:
		std::string appId;
		std::string name; // actual game name (shown in window and such)
		std::string organization;
		Version version;
	};

	struct EnvironmentInfo
	{
		Version engineVersion{0, 1, 0, "prealpha"};	 // version
		Version apiVersion;		 // graphic api version

		std::vector<std::string> commandLineArgs;
		std::unordered_map<std::string, std::string> environmentVars;

		SystemInfo system;
		PathInfo paths;

		ExecutionMode executionMode = ExecutionMode::Game;
		BuildConfiguration buildConfig = BuildConfiguration::Debug;

		void initialize(int argc, const char** argv);
	};

	auto signalToString(int signal) -> std::string;
}