SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";

{% for col in range(feat_tree.CaStorageDbSetting.Collections._value|length) -%}
    {%- set Name = feat_tree.CaStorageDbSetting.Collections.Name._value[col] or '' -%}
    {%- set GroupName = feat_tree.CaStorageDbSetting.Collections.GroupName._value[col] or '' -%}
    {%- set Flags = feat_tree.CaStorageDbSetting.Collections.Flags._value[col] or '' -%}
    {%- set LocName = feat_tree.CaStorageDbSetting.Collections.LocName._value[col] or ''-%}
    {%- set IconFileName = feat_tree.CaStorageDbSetting.Collections.IconFileName._value[col] or '' -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.Collections.IconSkinId._value[col] or '' -%}
INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME, FLAGS {% if LocName -%}, COL_LOCNAME{%- endif %}{% if IconFileName -%}, ICON_FILENAME{%- endif %}{% if IconSkinId -%}, ICON_SKIN_ID{%- endif %}) 
VALUES ( '{{Name}}', '{{GroupName}}', {{Flags}} {% if LocName -%},  "{{LocName}}"{%- endif %}{% if IconFileName -%}, "{{IconFileName}}"{%- endif %}{% if IconSkinId -%}, "{{IconSkinId}}"{%- endif %});

{% endfor %}

{% for index in range(feat_tree.CaStorageDbSetting.Items._value|length) -%}
    {%- set Type = feat_tree.CaStorageDbSetting.Items.Type._value[index] or '' -%}
    {%- set Name = feat_tree.CaStorageDbSetting.Items.Name._value[index] or '' -%}
    {%- set IconFileName = feat_tree.CaStorageDbSetting.Items.IconFileName._value[index] or '' -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.Items.IconSkinId._value[index] or '' -%}
    {%- set DestinationUid = feat_tree.CaStorageDbSetting.Items.DestinationUid._value[index] or '' -%}
    {%- set DestinationUrl = feat_tree.CaStorageDbSetting.Items.DestinationUrl._value[index] or '' -%}
    {%- set DestinationWidgetUri = feat_tree.CaStorageDbSetting.Items.DestinationWidgetUri._value[index] or '' -%}
    {%- set Collection = feat_tree.CaStorageDbSetting.Items.Col_Name._value[index] or '' -%}
{% if Type == "application" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("{{Type}}", '{{Name}}', {{DestinationUid}},  '{{Collection}}' );{%- endif %}
{% if Type == "url" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, ICON_FILENAME, ICON_SKIN_ID, COLLECTION_NAME) 
  VALUES ("{{Type}}", '{{Name}}', "{{DestinationUrl}}", "{{IconFileName}}", "{{IconSkinId}}",  '{{Collection}}' ); {%- endif %}
{% if Type == "widget" -%} INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON_FILENAME, ICON_SKIN_ID, URI, COLLECTION_NAME ) 
  VALUES ("{{Type}}", '{{Name}}', "{{IconFileName}}", "{{IconSkinId}}", "{{DestinationWidgetUri}}", '{{Collection}}' ); {%- endif %}
{% endfor %}

SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

