﻿#include "song.hh"
#include "songparser.hh"
#include "notes.hh"
#include "util.hh"
#include <limits>
#include <algorithm>

void Song::reload(bool errorIgnore) {
	loadStatus = NONE;
	vocalTracks.clear();
	//instrumentTracks.clear();
	//danceTracks.clear();
	beats.clear();
	midifilename.clear();
	category.clear();
	genre.clear();
	edition.clear();
	title.clear();
	artist.clear();
	collateByTitle.clear();
	collateByTitleOnly.clear();
	collateByArtist.clear();
	collateByArtistOnly.clear();
	text.clear();
	creator.clear();
	music.clear();
	cover.clear();
	background.clear();
	video.clear();
	videoGap = 0.0;
	start = 0.0;
	preview_start = getNaN();
	bpm = 0.0;
	hasBRE = false;
	b0rkedTracks = false;
	if (!filename.isEmpty()) {
		try { SongParser(*this); } catch (...) { if (!errorIgnore) throw; }
	}
	collateUpdate();
	insertVocalTrack(TrackName::LEAD_VOCAL, VocalTrack(TrackName::LEAD_VOCAL)); //to avoid the editor dumping notes in both tracks at startup
	insertVocalTrack(TrackName::DUET_P2, VocalTrack(TrackName::DUET_P2));
}

void Song::dropNotes() {
	// Singing
	if (!vocalTracks.empty()) {
		for (VocalTracks::iterator it = vocalTracks.begin(); it != vocalTracks.end(); ++it)
			it->second.notes.clear();
	}
	/*
	// Instruments
	if (!instrumentTracks.empty()) {
		for (InstrumentTracks::iterator it = instrumentTracks.begin(); it != instrumentTracks.end(); ++it)
			it->second.nm.clear();
	}
	// Dancing
	if (!danceTracks.empty()) {
		for (DanceTracks::iterator it = danceTracks.begin(); it != danceTracks.end(); ++it)
			it->second.clear();
	}
	*/
	b0rkedTracks = false;
	loadStatus = HEADER;
}

void Song::collateUpdate() {
	collateByTitle = collate(title + artist) + '\0' + filename;
	collateByTitleOnly = collate(title);
	collateByArtist = collate(artist + title) + '\0' + filename;
	collateByArtistOnly = collate(artist);
}

QString Song::collate(QString const& str) {
	return str; //unicodeCollate(str);
}

namespace {
	// Cannot simply take double as its second argument because of a C++ defect
	bool noteEndLessThan(Note const& a, Note const& b) { return a.end < b.end; }
}

Song::Status Song::status(double time) {
	Note target; target.end = time;
	Notes::const_iterator it = std::lower_bound(getVocalTrack().notes.begin(), getVocalTrack().notes.end(), target, noteEndLessThan);
	if (it == getVocalTrack().notes.end()) return FINISHED;
	if (it->begin > time + 4.0) return INSTRUMENTAL_BREAK;
	return NORMAL;
}

bool Song::getNextSection(double pos, SongSection &section) {
	if (songsections.empty()) return false;
	for (std::vector<Song::SongSection>::iterator it= songsections.begin(); it != songsections.end(); ++it) {
		if (it->begin > pos) {
			section = *it;
			return true;
		}
	}
	// returning false here will jump forward 5s (see screen_sing.cc)
	return false;
}

bool Song::getPrevSection(double pos, SongSection &section) {
	if (songsections.empty()) return false;
	for (std::vector<Song::SongSection>::reverse_iterator it= songsections.rbegin(); it != songsections.rend(); it++) {
		// subtract 1 second so we can jump across a section
		if (it->begin < pos - 1.0) {
			section = *it;
			return true;
		}
	}
	// returning false here will jump backwards by 5s (see screen_sing.cc)
	return false;
}
