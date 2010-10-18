DROP TABLE IF EXISTS CA_ENTRY;
DROP TABLE IF EXISTS CA_GROUP_ENTRY;
DROP TABLE IF EXISTS CA_ICON;
DROP TABLE IF EXISTS CA_LAUNCH;
DROP TABLE IF EXISTS CA_ATTRIBUTE;
DROP TABLE IF EXISTS CA_LOCALIZATION_TEXT;
DROP TABLE IF EXISTS CA_DB_PROPERTIES;

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
                                  IC_SKIN_ID TEXT,
                                  IC_APP_ID TEXT,
                                  UNIQUE( IC_FILENAME, IC_SKIN_ID, IC_APP_ID ) );

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
                                  LT_QMFILENAME TEXT,
                                  LT_ROW_ID INTEGER );
                                  
SELECT "CREATE TABLE CA_DB_PROPERTIES" AS " ";
SELECT "------------------------------------" AS " ";
CREATE TABLE CA_DB_PROPERTIES( DB_PROPERTIES_ID INTEGER PRIMARY KEY ,
                                  DB_PROPERTY TEXT UNIQUE,
                                  DB_VALUE TEXT );       
                                   

SELECT "INSERT INITIAL DATA TO CA_DB_PROPERTIES" AS " ";

SELECT "------------------------------------" AS " ";
INSERT INTO CA_DB_PROPERTIES ( DB_PROPERTY, DB_VALUE ) 
VALUES ( 'Version', '00001' );
INSERT INTO CA_DB_PROPERTIES ( DB_PROPERTY, DB_VALUE ) 
VALUES ( 'Language', '' );
INSERT INTO CA_DB_PROPERTIES ( DB_PROPERTY, DB_VALUE ) 
VALUES ( 'QMfile', 'contentstorage_' );

-- CREATE VIEW COLLECTION
SELECT "------------------------------------" AS " ";
SELECT "CREATE VIEW COLLECTION" AS " ";
DROP VIEW IF EXISTS COLLECTION;
CREATE VIEW COLLECTION 
  AS SELECT 
    "" AS "COLLECTION_NAME",
    "" AS "COL_LOCNAME",
    "" AS "TRANSLATION_FILE",
    "" AS "COL_SHORT_NAME",
    "" AS "COL_TITLE_NAME",
    "" AS "COL_APP_GROUP_NAME", 
    "" AS "ICON_FILENAME",
    "" AS "ICON_SKIN_ID",
    "" AS "FLAGS";
    
SELECT "  CREATE TRIGGER collection_insert" AS " ";
CREATE TRIGGER collection_insert INSTEAD OF INSERT ON COLLECTION 
BEGIN  
  -- INSERT ICON
  INSERT INTO CA_ICON (IC_FILENAME, IC_SKIN_ID) 
  VALUES ( new.ICON_FILENAME, new.ICON_SKIN_ID);
  -- DELETE IF ICON IS NULL OR IF IT IS DUPLICATE
  DELETE FROM CA_ICON WHERE ICON_ID = last_insert_rowid() AND 
      ( (IC_FILENAME = "" AND IC_SKIN_ID ="") OR 
        (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME AND IC_SKIN_ID = new.ICON_SKIN_ID AND ICON_ID <> last_insert_rowid()) IS NOT NULL);
  -- INSERT ENTRY
    INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_FLAGS, EN_ICON_ID ) 
    VALUES ( new.COLLECTION_NAME , 2, "collection",  new.FLAGS, (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME AND IC_SKIN_ID = new.ICON_SKIN_ID ));
    -- LOCALIZATION 
    INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_QMFILENAME, LT_ROW_ID ) 
    VALUES ( 'CA_ENTRY', 'EN_TEXT', new.COLLECTION_NAME, new.TRANSLATION_FILE, ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ) );
    DELETE FROM CA_LOCALIZATION_TEXT WHERE LT_QMFILENAME IS NULL AND LOC_TEXT_ID = last_insert_rowid() ;
    INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_QMFILENAME, LT_ROW_ID ) 
    VALUES ( 'CA_ATTRIBUTE', 'short_name', new.COL_SHORT_NAME, new.TRANSLATION_FILE, ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ) );
    DELETE FROM CA_LOCALIZATION_TEXT WHERE ( LT_QMFILENAME IS NULL OR LT_STRING_ID IS NULL ) AND LOC_TEXT_ID = last_insert_rowid();
    INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_QMFILENAME, LT_ROW_ID ) 
    VALUES ( 'CA_ATTRIBUTE', 'title_name', new.COL_TITLE_NAME, new.TRANSLATION_FILE, ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ) );
    DELETE FROM CA_LOCALIZATION_TEXT WHERE ( LT_QMFILENAME IS NULL OR LT_STRING_ID IS NULL ) AND LOC_TEXT_ID = last_insert_rowid();
    -- RELATION TO MAIN COLLECTION (ADD TO GROUP: )
  INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
  VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "menucollections") , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME), 
          (SELECT IFNULL(MAX(GE_POSITION)+1, 1) FROM CA_GROUP_ENTRY WHERE GE_GROUP_ID = (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "menucollections")) );
    -- ADD ATTRIBUTES
    INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )
    VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME AND (EN_TYPE_NAME = "collection::downloaded" OR EN_TYPE_NAME = "collection") ), "appgroup_name", new.COL_APP_GROUP_NAME );
    INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )
    VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME AND (EN_TYPE_NAME = "collection::downloaded" OR EN_TYPE_NAME = "collection") ), "short_name", new.COL_SHORT_NAME );
    INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )
    VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME AND (EN_TYPE_NAME = "collection::downloaded" OR EN_TYPE_NAME = "collection") ), "title_name", new.COL_TITLE_NAME );
END;

-- CREATE VIEW ITEM_TO_COLLECTION
SELECT "------------------------------------" AS " ";
SELECT "CREATE VIEW ITEM_TO_COLLECTION" AS " ";
DROP VIEW IF EXISTS ITEM_TO_COLLECTION;
CREATE VIEW ITEM_TO_COLLECTION 
  AS SELECT 
    "" AS "ITEM_TYPE",
    "" AS "ITEM_NAME",
    "" AS "ITEM_SHORTNAME",
    "" AS "ICON_FILENAME",
    "" AS "ICON_SKIN_ID",
    "" AS "ITEM_DEST",    
    "" AS "COLLECTION_NAME";

SELECT "  CREATE TRIGGER item_to_collection_insert_app" AS " ";
CREATE TRIGGER item_to_collection_insert_app INSTEAD OF INSERT ON ITEM_TO_COLLECTION WHEN new.ITEM_TYPE="application"
BEGIN
  -- INSERT ENTRY
  INSERT OR IGNORE INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_UID ) 
  VALUES ( new.ITEM_NAME, 1, "application", new.ITEM_DEST   );
  -- RELATION TO MAIN COLLECTION (ADD TO GROUP: )
  INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
  VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME) , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_UID = new.ITEM_DEST), 
          (SELECT IFNULL(MAX(GE_POSITION)+1, 1) FROM CA_GROUP_ENTRY WHERE GE_GROUP_ID = (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME)) );
END;

SELECT "  CREATE TRIGGER item_to_collection_insert_url" AS " ";
CREATE TRIGGER item_to_collection_insert_url INSTEAD OF INSERT ON ITEM_TO_COLLECTION WHEN new.ITEM_TYPE="url"
BEGIN
  -- RELATION TO MAIN COLLECTION (ADD TO GROUP: )
  INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
  VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME) , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE (EN_TEXT = new.ITEM_NAME AND EN_TYPE_NAME = new.ITEM_TYPE )),
          (SELECT IFNULL(MAX(GE_POSITION)+1, 1) FROM CA_GROUP_ENTRY WHERE GE_GROUP_ID = (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME)) );
  -- UPDATE FLAGS IN A COLLECTION TO WHICH THE URL IS INSERTED
  UPDATE CA_ENTRY
        SET EN_FLAGS=EN_FLAGS|8
        WHERE EN_TEXT=new.COLLECTION_NAME;
END;


-- CREATE VIEW WIDGET   _TO_COLLECTION
SELECT "------------------------------------" AS " ";
SELECT "CREATE VIEW WIDGET_TO_COLLECTION" AS " ";
DROP VIEW IF EXISTS WIDGET_TO_COLLECTION;
CREATE VIEW WIDGET_TO_COLLECTION 
  AS SELECT 
    "" AS "ITEM_TYPE",
    "" AS "ITEM_NAME",
    "" AS "ICON_FILENAME",
    "" AS "ICON_SKIN_ID",
    "" AS "PACKAGE_UID",
    "" AS "LIBRARY",
    "" AS "URI",
    "" AS "COLLECTION_NAME";

SELECT "  CREATE TRIGGER insert_widget_to_collection" AS " ";
CREATE TRIGGER item_to_collection_insert_widget INSTEAD OF INSERT ON WIDGET_TO_COLLECTION WHEN new.ITEM_TYPE="widget"
BEGIN
  -- INSERT ICON
  INSERT OR IGNORE INTO CA_ICON (IC_FILENAME, IC_SKIN_ID) 
  VALUES ( new.ICON_FILENAME, new.ICON_SKIN_ID);
  DELETE FROM CA_ICON WHERE ICON_ID = last_insert_rowid() AND 
      ( (IC_FILENAME = "" AND IC_SKIN_ID = "") OR 
        (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME AND IC_SKIN_ID = new.ICON_SKIN_ID AND ICON_ID <> last_insert_rowid()) IS NOT NULL);
  -- INSERT ENTRY
  INSERT OR IGNORE INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_FLAGS, EN_ICON_ID ) 
  VALUES ( new.ITEM_NAME, 1, "widget", 0, (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME AND IC_SKIN_ID = new.ICON_SKIN_ID ));
  -- INSERT ATTRIBUTES
  INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
  VALUES ( ( SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.ITEM_NAME ) ,"packageuid",new.PACKAGE_UID);
  INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
  VALUES ( ( SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.ITEM_NAME ) ,"widget:library",new.LIBRARY);
  INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
  VALUES ( ( SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.ITEM_NAME ) ,"widget:uri",new.URI);
  -- RELATION TO MAIN COLLECTION (ADD TO GROUP: )
  INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
  VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME) , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE (EN_TEXT = new.ITEM_NAME AND EN_TYPE_NAME = new.ITEM_TYPE)), 
          (SELECT IFNULL(MAX(GE_POSITION)+1, 1) FROM CA_GROUP_ENTRY WHERE GE_GROUP_ID = (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.COLLECTION_NAME)) );
END;



-- CREATE VIEW URL
SELECT "------------------------------------" AS " ";
SELECT "CREATE VIEW URL" AS " ";
DROP VIEW IF EXISTS URL;
CREATE VIEW URL 
  AS SELECT 
    "" AS "URL_TITLE",
    "" AS "URL_SHORT_TITLE",
    "" AS "ICON_FILENAME",
    "" AS "ICON_SKIN_ID",
    "" AS "URL_DEST";

SELECT "  CREATE TRIGGER insert_url" AS " ";
CREATE TRIGGER insert_url INSTEAD OF INSERT ON URL
BEGIN
  -- INSERT ICON
  INSERT OR IGNORE INTO CA_ICON (IC_FILENAME, IC_SKIN_ID) 
  VALUES ( new.ICON_FILENAME, new.ICON_SKIN_ID);
  DELETE FROM CA_ICON WHERE ICON_ID = last_insert_rowid() AND 
      ( (IC_FILENAME = "" AND IC_SKIN_ID = "") OR 
        (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME  AND IC_SKIN_ID = new.ICON_SKIN_ID AND ICON_ID <> last_insert_rowid()) IS NOT NULL);  
  -- INSERT ENTRY
  INSERT OR IGNORE INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID ) 
  VALUES ( new.URL_TITLE, 1, "url", (SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = new.ICON_FILENAME AND IC_SKIN_ID = new.ICON_SKIN_ID ) );
  -- ADD ATTRIBUTES
  INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
  VALUES ( ( SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.URL_TITLE ), "url", new.URL_DEST );
  INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
  VALUES ( ( SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TEXT = new.URL_TITLE ) ,"short_name", new.URL_SHORT_TITLE );
END;



-- insert neccessary items
SELECT "INSERT menucollections ENTRY" AS " ";
INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID, EN_UID ) 
VALUES ( 'AllCollections', 2, "menucollections", NULL , NULL  );

INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
VALUES ( last_insert_rowid(), "appgroup_name", "AllCollections" );

SELECT "------------------------------------" AS " ";
SELECT "INSERT Downloaded collection ENTRY" AS " ";
INSERT INTO CA_ICON (IC_FILENAME, IC_SKIN_ID) 
VALUES ( "qtg_large_applications_download", "" );

INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID,  EN_FLAGS ) 
VALUES ( 'txt_applib_dblist_downloads', 2, "collection::downloaded", ( SELECT MAX(ICON_ID) FROM CA_ICON ) , 8 );

INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
VALUES ( ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ), "appgroup_name", "Downloaded" );

INSERT OR IGNORE INTO CA_ATTRIBUTE (AT_ENTRY_ID, AT_NAME, AT_VALUE )  
VALUES ( ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ), "title_name", "txt_applib_subtitle_downloads" );

INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_QMFILENAME, LT_ROW_ID ) 
VALUES ( 'CA_ENTRY', 'EN_TEXT', "txt_applib_dblist_downloads",( SELECT DB_VALUE FROM CA_DB_PROPERTIES WHERE DB_PROPERTY = "QMfile" ), ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ) );

INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_QMFILENAME, LT_ROW_ID ) 
VALUES ( 'CA_ATTRIBUTE', 'title_name', "txt_applib_subtitle_downloads",( SELECT DB_VALUE FROM CA_DB_PROPERTIES WHERE DB_PROPERTY = "QMfile" ), ( SELECT MAX(ENTRY_ID) FROM CA_ENTRY ) );

INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "menucollections") , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "collection::downloaded"), 
          1 ); 

          
          
SELECT "------------------------------------" AS " ";
SELECT "castoragedb_create - END" AS " ";
SELECT "------------------------------------" AS " ";

