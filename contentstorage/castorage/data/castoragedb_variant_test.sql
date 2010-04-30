-- ONLY IF YOU SURE THAT WANT DEFAULT CONTENT
.read castoragedb_variant.sql

SELECT "castoragedb_variant test - BEGIN" AS " ";


-- insert collection
INSERT INTO COLLECTION (COLLECTION_NAME, ICON) VALUES ( 'Test Apps', "qtg_large_applications_user" );

-- insert example urls
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("url", 'Facebook', 'c:\data\Images\kqticaicons\facebook.png', "http://www.facebook.com/", 'Test Apps' );
  INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("url", 'Google', 'c:\data\Images\kqticaicons\google.png', "http://www.google.com/", 'Test Apps' );

-- insert example widgets
  INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE, ITEM_NAME,ICON, PACKAGE_UID, LIBRARY, LONG_NAME, URI, COLLECTION_NAME) 
  VALUES ("widget", 'Clock', 'Z:\private\20022F35\import\widgetregistry\20022F6C\tclock.png', '20022f6c', 'Z:\private\20022F35\import\widgetregistry\20022F6C\hsclockwidgetplugin', 'Clock', 'hsclockwidgetplugin', 'Test Apps' ); 


SELECT "castoragedb_variant test - END" AS " ";