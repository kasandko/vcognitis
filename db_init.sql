PRAGMA encoding = 'UTF-8';

CREATE TABLE IF NOT EXISTS Groups (
        id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
        screenName TEXT NOT NULL UNIQUE ON CONFLICT IGNORE,
        name TEXT,
        photo TEXT,
        priority INTEGER,
        weight INTEGER,
        subCount INTEGER,
        subHidden INTEGER DEFAULT 0,
        offset INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS Countries (
        id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
        name TEXT
);

CREATE TABLE IF NOT EXISTS InterestCategory (
        id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
        name TEXT NOT NULL,
        tags TEXT
);

CREATE TABLE IF NOT EXISTS Interest (
        id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
        name TEXT NOT NULL,
        tags TEXT,
        parentId INTEGER,
        FOREIGN KEY(parentId) REFERENCES InterestCategory(id) ON DELETE SET NULL ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS Users (
        id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
        screenName TEXT,
        firstName TEXT,
        lastName TEXT,
        cityId INTEGER,
        countryId INTEGER,
        sex INTEGER,
        bdate INTEGER,
        photoMax TEXT,
        relation INTEGER,
        religion INTEGER,
        political INTEGER,
        peopleMain INTEGER,
        lifeMain INTEGER,
        smoking INTEGER,
        alcohol INTEGER,
        groupScore INTEGER DEFAULT 0,
        interestScore INTEGER DEFAULT 0,
        scored INTEGER DEFAULT 0,
        favorite BOOLEAN DEFAULT 0,
        inaccurateAge BOOLEAN DEFAULT 0,
        groupId INTEGER,
        openMessages INTEGER,
        viewed BOOLEAN DEFAULT 0,
        FOREIGN KEY(countryId) REFERENCES Countries(id) ON DELETE SET NULL ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS UserGroups (
        userId INTEGER NOT NULL,
        groupId INTEGER NOT NULL,
        PRIMARY KEY (userId, groupId) ON CONFLICT IGNORE,
        FOREIGN KEY(userId) REFERENCES Users(id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS UserInterest (
        userId INTEGER NOT NULL,
        interestId INTEGER NOT NULL,
        countGroups INTEGER NOT NULL,
        PRIMARY KEY (userId, interestId) ON CONFLICT IGNORE,
        FOREIGN KEY(userId) REFERENCES Users(id) ON DELETE CASCADE ON UPDATE CASCADE,
        FOREIGN KEY(interestId) REFERENCES Interest(id) ON DELETE CASCADE ON UPDATE CASCADE
);

PRAGMA synchronous = NORMAL;
PRAGMA foreign_keys = ON;
