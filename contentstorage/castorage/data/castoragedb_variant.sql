SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";


-- insert additional items

INSERT INTO COLLECTION (COLLECTION_NAME, COL_LOCNAME, COL_APP_GROUP_NAME ,ICON, FLAGS) VALUES ( 'Office',  "txt_applib_dblist_office", 'Office', "qtg_large_applications_user", 4 );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'calendar', 268458241,  'Office' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Phonebook', 537014009,  'Office' );

INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME ,ICON, FLAGS) VALUES ( 'Multimedia', 'Multimedia' , "qtg_large_applications_user", 4 );  
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Themes', 268458546,  'Multimedia' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Paint Cursor', 537001564,  'Multimedia' ); 
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Videoplayer', 537006590,  'Multimedia' );

INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME ,ICON, FLAGS) VALUES ( 'Utils', 'Utils', "qtg_large_applications_user", 4 );  
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
  
INSERT INTO COLLECTION (COLLECTION_NAME, COL_LOCNAME, COL_APP_GROUP_NAME ,ICON) VALUES ( 'Games', "txt_applib_dblist_games", 'Games',"qtg_large_applications_user", 4 );

SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

