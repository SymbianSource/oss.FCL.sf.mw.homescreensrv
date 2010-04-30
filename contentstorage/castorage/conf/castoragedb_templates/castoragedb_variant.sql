SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";

{% for col in range(feat_tree.CaStorageDbSetting.Collections._value|length) -%}
    {%- set Name = feat_tree.CaStorageDbSetting.Collections.Name._value[col] or '' -%}
    {%- set GroupName = feat_tree.CaStorageDbSetting.Collections.GroupName._value[col] or '' -%}
    {%- set Flags = feat_tree.CaStorageDbSetting.Collections.Flags._value[col] or '' -%}
    {%- set LocName = feat_tree.CaStorageDbSetting.Collections.LocName._value[col] or ''-%}
    {%- set Icon = feat_tree.CaStorageDbSetting.Collections.Icon._value[col] or '' -%}
INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME, FLAGS {% if LocName -%}, COL_LOCNAME{%- endif %}{% if Icon -%}, ICON{%- endif %}) 
VALUES ( '{{Name}}', '{{GroupName}}', {{Flags}} {% if LocName -%},  "{{LocName}}"{%- endif %}{% if Icon -%}, "{{Icon}}"{%- endif %});

{% endfor %}

{% for index in range(feat_tree.CaStorageDbSetting.Items._value|length) -%}
    {%- set Type = feat_tree.CaStorageDbSetting.Items.Type._value[index] or '' -%}
    {%- set Name = feat_tree.CaStorageDbSetting.Items.Name._value[index] or '' -%}
    {%- set Icon = feat_tree.CaStorageDbSetting.Items.Icon._value[index] or '' -%}
    {%- set Destination = feat_tree.CaStorageDbSetting.Items.Destination._value[index] or '' -%}
    {%- set Collection = feat_tree.CaStorageDbSetting.Items.Col_Name._value[index] or '' -%}
{% if Type == "application" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE,ITEM_NAME,ITEM_DEST, COLLECTION_NAME) 
  VALUES ("{{Type}}", '{{Name}}', {{Destination}},  '{{Collection}}' );{%- endif %}
{% if Type == "url" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, ICON, COLLECTION_NAME) 
  VALUES ("{{Type}}", '{{Name}}', "{{Destination}}", "{{Icon}}",  '{{Collection}}' ); {%- endif %}
{% if Type == "widget" -%} INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON, URI, COLLECTION_NAME ) 
  VALUES ("{{Type}}", '{{Name}}', "{{Icon}}", "{{Destination}}", '{{Collection}}' ); {%- endif %}
{% endfor %}

SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

