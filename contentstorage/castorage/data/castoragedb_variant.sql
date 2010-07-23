SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";


-- insert additional items

INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME, ICON_FILENAME, FLAGS) VALUES ( 'Favorites', 'Favorites', "qtg_large_applications_user", 4 );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'calendar', 268458241,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Camera', 270501242,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Contacts', 537014009,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Email', 537023930,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'FM Radio', 270530934,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Internet', 222222222,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Maps', 222222223,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Messaging', 537001593,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Music', 270564450,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Photos (camera)', 536913858,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Settings', 268458220,  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", 'Videos (video camera)', 222222224,  'Favorites' );


SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

