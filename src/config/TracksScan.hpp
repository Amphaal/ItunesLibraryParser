// ItunesLibraryParser
// Allows JSON parsing of XML Itunes Library file
// Copyright (C) 2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

/* UPDATE THIS FILE */

#pragma once

#include "Tracks.hpp"

namespace FieldType {

constexpr const FieldTypeStruct<0, "Track ID",       "<key>Track ID</key><integer>",       "</integer>">   TrackID;
constexpr const FieldTypeStruct<1, "Disc Number",    "<key>Disc Number</key><integer>",    "</integer>">   DiscNumber;
constexpr const FieldTypeStruct<2, "Track Number",   "<key>Track Number</key><integer>",   "</integer>">   TrackNumber;
constexpr const FieldTypeStruct<3, "Year",           "<key>Year</key><integer>",           "</integer>">   Year;
constexpr const FieldTypeStruct<4, "Date Added",     "<key>Date Added</key><date>",        "</date>">      DateAdded;
constexpr const FieldTypeStruct<5, "Name",           "<key>Name</key><string>",            "</string>">    Name;
constexpr const FieldTypeStruct<6, "Album Artist",   "<key>Album Artist</key><string>",    "</string>">    AlbumArtist;
constexpr const FieldTypeStruct<7, "Album",          "<key>Album</key><string>",           "</string>">    Album;
constexpr const FieldTypeStruct<8, "Genre",          "<key>Genre</key><string>",           "</string>">    Genre;

constexpr OrderedScansT orderedScans {
    &TrackID,
    &DiscNumber,
    &TrackNumber,
    &Year,
    &DateAdded,
    &Name,
    &AlbumArtist,
    &Album,
    &Genre,
};

}  // namespace FieldType
