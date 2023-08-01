#pragma once

// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#include "PBR_include/parser.h"

#include <string>
#include <vector>

namespace PBR {

	// File and Filename Function Declarations
	std::string ReadFileContents(std::string filename);
	std::string ReadDecompressedFileContents(std::string filename);
	bool WriteFileContents(std::string filename, const std::string& contents);

//	std::vector<float> ReadFloatFile(std::string filename);

	bool FileExists(std::string filename);
	bool RemoveFile(std::string filename);

	std::string ResolveFilename(std::string filename);
	void SetSearchDirectory(std::string filename);

	bool HasExtension(std::string filename, std::string ext);
	std::string RemoveExtension(std::string filename);

	std::vector<std::string> MatchingFilenames(std::string filename);

	FILE* FOpenRead(std::string filename);
	FILE* FOpenWrite(std::string filename);

}  // namespace pbrt


