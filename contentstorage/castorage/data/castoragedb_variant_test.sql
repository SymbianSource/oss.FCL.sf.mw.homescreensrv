-- ONLY IF YOU SURE THAT WANT DEFAULT CONTENT
.read castoragedb_variant.sql

SELECT "castoragedb_variant test - BEGIN" AS " ";


-- insert collection
INSERT INTO COLLECTION (COLLECTION_NAME, ICON) VALUES ( 'Favorites', "qtg_large_applications_user" );

-- insert example urls
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ICON,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("url", 'Facebook', 'c:\data\Images\kqticaicons\facebook.png', "http://www.facebook.com/",  'Favorites' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ICON,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("url", 'Google', 'c:\data\Images\kqticaicons\google.png', "http://www.google.com/",  'Favorites' );

-- insert example widgets
  INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ICON, PACKAGE_UID, LIBRARY, LONG_NAME, URI, COLLECTION_NAME) 
  VALUES ("widget", 'aClock', 'Z:\private\20022F35\import\widgetregistry\20022F6C\tclock.png',  '20022f43', 'Z:\private\20022F35\import\widgetregistry\20022F43\hsclockwidgetprovider.dll', 'Clock', 'homescreen.nokia.com/widget/clock', 'Office' ); 
  


SELECT "castoragedb_variant test - END" AS " ";