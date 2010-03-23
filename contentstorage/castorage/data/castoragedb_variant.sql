SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";

-- insert neccessary items
SELECT "INSERT GROUPS" AS " ";
SELECT "." AS " ";
INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID, EN_UID ) 
VALUES ( 'Menu', 2, "folder", NULL , NULL  );

SELECT "INSERT menucollections ENTRY" AS " ";
INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID, EN_UID ) 
VALUES ( 'AllCollections', 2, "menucollections", NULL , NULL  );

INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID, EN_UID, EN_FLAGS ) 
VALUES ( 'Application Library', 2, "applicationlibrary", NULL, NULL, 8 );

SELECT "------------------------------------" AS " ";
SELECT "INSERT Downloaded collection ENTRY" AS " ";
INSERT INTO CA_ICON (IC_FILENAME) 
VALUES ( "qtg_large_applications_download");
INSERT INTO CA_ENTRY (EN_TEXT, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID,  EN_FLAGS ) 
VALUES ( 'Downloaded', 2, "collection::downloaded", last_insert_rowid()  , 8 );
INSERT INTO CA_LOCALIZATION_TEXT ( LT_TABLE_NAME, LT_ATTRIBUTE_NAME, LT_STRING_ID, LT_ROW_ID ) 
VALUES ( 'CA_ENTRY', 'EN_TEXT', "txt_applib_dblist_downloaded", ( SELECT last_insert_rowid() ) );
INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID, GE_ENTRY_ID, GE_POSITION) 
VALUES ( (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "menucollections") , 
          (SELECT ENTRY_ID FROM CA_ENTRY WHERE EN_TYPE_NAME = "collection::downloaded"), 
          1 ); 

-- insert additional items

INSERT INTO COLLECTION (COLLECTION_NAME, COL_LOCNAME, ICON) VALUES ( 'Office',  "txt_applib_dblist_office", "qtg_large_applications_user");
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'calendar', 268458241,  'Office' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Phonebook', 537014009,  'Office' );

INSERT INTO COLLECTION (COLLECTION_NAME, ICON) VALUES ( 'Multimedia', "qtg_large_applications_user" );  
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Themes', 268458546,  'Multimedia' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Paint Cursor', 537001564,  'Multimedia' ); 
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Videoplayer', 537006590,  'Multimedia' );

INSERT INTO COLLECTION (COLLECTION_NAME, ICON) VALUES ( 'Utils', "qtg_large_applications_user" );  
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'ControlPanel', 537026521,  'Utils' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Bluetooth', 268458321,  'Utils' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'SW update', 537001519,  'Utils' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'eshell', 270498535,  'Utils' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Conn. mgr.', 270501072,  'Utils' );

  
INSERT INTO COLLECTION (COLLECTION_NAME, COL_LOCNAME, ICON) VALUES ( 'Games', "txt_applib_dblist_games", "qtg_large_applications_user" );

SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

