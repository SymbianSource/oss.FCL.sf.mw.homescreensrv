DROP TABLE IF EXISTS CA_ENTRY;
DROP TABLE IF EXISTS CA_GROUP_ENTRY;
DROP TABLE IF EXISTS CA_ICON;
DROP TABLE IF EXISTS CA_LAUNCH;
DROP TABLE IF EXISTS CA_ATTRIBUTE;

SELECT "CREATE TABLE CA_ENTRY" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_ENTRY( ENTRY_ID INTEGER PRIMARY KEY,
                                 EN_TEXT TEXT NOT NULL COLLATE NOCASE,
                                 EN_DESCRIPTION TEXT COLLATE NOCASE,
                                 EN_ROLE INTEGER default 1,
                                 EN_TYPE_NAME TEXT NOT NULL,
                                 EN_ICON_ID INTEGER,
                                 EN_CREATION_TIME BIGINT default -1 NOT NULL,
                                 EN_UID INTEGER UNIQUE, 
                                 EN_FLAGS INTEGER default 8, 
                                 FOREIGN KEY(EN_ICON_ID) REFERENCES CA_ICON(ICON_ID) );
CREATE INDEX EntryText_CAIndex ON CA_ENTRY(EN_TEXT);
CREATE INDEX EntryCreationTime_CAIndex ON CA_ENTRY(EN_CREATION_TIME);

SELECT "CREATE TABLE CA_GROUP_ENTRY" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_GROUP_ENTRY( GROUP_ENTRY_ID INTEGER PRIMARY KEY ,
                                  GE_GROUP_ID INTEGER NOT NULL,
                                  GE_ENTRY_ID INTEGER NOT NULL,
                                  GE_POSITION INTEGER,
                                  UNIQUE( GE_GROUP_ID, GE_ENTRY_ID ),
                                  FOREIGN KEY(GE_GROUP_ID) REFERENCES CA_ENTRY(ENTRY_ID),
                                  FOREIGN KEY(GE_ENTRY_ID) REFERENCES CA_ENTRY(ENTRY_ID) );
CREATE INDEX GroupGE_POSITION_CAIndex ON CA_GROUP_ENTRY(GE_POSITION);
CREATE TRIGGER update_position_in_ca_group_entry_after_remove AFTER DELETE ON CA_GROUP_ENTRY
BEGIN
    UPDATE CA_GROUP_ENTRY SET GE_POSITION = GE_POSITION - 1 WHERE GE_POSITION > old.GE_POSITION AND GE_GROUP_ID = old.GE_GROUP_ID;
END; 
CREATE TRIGGER update_position_in_ca_group_entry_before_insert BEFORE INSERT ON CA_GROUP_ENTRY
BEGIN
    UPDATE CA_GROUP_ENTRY SET GE_POSITION = GE_POSITION + 1 WHERE GE_POSITION >= new.GE_POSITION AND GE_GROUP_ID = new.GE_GROUP_ID;
END;

SELECT "CREATE TABLE CA_ICON" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_ICON( ICON_ID INTEGER PRIMARY KEY ,
                                  IC_FILENAME TEXT,
                                  IC_BITMAP_ID INTEGER,
                                  IC_MASK_ID INTEGER,
                                  IC_SKINMAJOR_ID INTEGER,
                                  IC_SKINMINOR_ID INTEGER,
                                  UNIQUE( IC_FILENAME, IC_BITMAP_ID, IC_MASK_ID, IC_SKINMAJOR_ID, IC_SKINMINOR_ID) );

SELECT "CREATE TABLE CA_LAUNCH" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_LAUNCH( LAUNCH_ID INTEGER PRIMARY KEY ,
                                  LA_ENTRY_ID INTEGER NOT NULL,
                                  LA_LAUNCH_TIME INTEGER NOT NULL,
                                  FOREIGN KEY(LA_ENTRY_ID) REFERENCES CA_ENTRY(ENTRY_ID) );
CREATE INDEX LaunchLaunchTime_CAIndex ON CA_LAUNCH(LA_LAUNCH_TIME);

SELECT "CREATE TABLE CA_ATTRIBUTE" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_ATTRIBUTE( ATTRIBUTE_ID INTEGER PRIMARY KEY ,
                                  AT_ENTRY_ID INTEGER NOT NULL,
                                  AT_NAME TEXT NOT NULL,
                                  AT_VALUE TEXT NOT NULL,
                                  UNIQUE( AT_ENTRY_ID, AT_NAME ),
                                  FOREIGN KEY(AT_ENTRY_ID) REFERENCES CA_ENTRY(ENTRY_ID) );

SELECT "CREATE TABLE CA_LOCALIZATION_TEXT" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_LOCALIZATION_TEXT( LOC_TEXT_ID INTEGER PRIMARY KEY ,
                                  LT_TABLE_NAME TEXT ,
                                  LT_ATTRIBUTE_NAME TEXT ,
                                  LT_STRING_ID TEXT ,
                                  LT_ROW_ID INTEGER );
                                  
SELECT "CREATE TABLE CA_DB_PROPERTIES" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_DB_PROPERTIES( DB_PROPERTIES_ID INTEGER PRIMARY KEY ,
                                  DB_PROPERTY TEXT UNIQUE,
                                  DB_VALUE TEXT );
