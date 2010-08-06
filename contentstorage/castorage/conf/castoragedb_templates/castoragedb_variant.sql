SELECT "------------------------------------" AS " ";
SELECT "castoragedb_variant - BEGIN" AS " ";

{% for col in range(feat_tree.CaStorageDbSetting.OperatorCollections._value|length) -%}
    {%- set Name = feat_tree.CaStorageDbSetting.OperatorCollections.Name._value[col] or '' -%}
    {%- set ShortName = feat_tree.CaStorageDbSetting.OperatorCollections.ShortName._value[col] or '' -%}
    {%- set TitleName = feat_tree.CaStorageDbSetting.OperatorCollections.TitleName._value[col] or '' -%}
    {%- set GroupName = feat_tree.CaStorageDbSetting.OperatorCollections.GroupName._value[col] or '' -%}
    {%- set Locked = feat_tree.CaStorageDbSetting.OperatorCollections.Locked._value[col] or false -%}
    {%- set IconFileName = ( feat_tree.CaStorageDbSetting.OperatorCollections.Icon.localPath._value[col] or '' ) -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.OperatorCollections.IconSkinId._value[col] or '' -%}
    {% if IconSkinId == "" and IconFileName != "" and IconFileName != "dummy" -%}
        {% set IconFileName = "z:\\private\\20022F35\\customsvg\\" + IconFileName %}
    {% endif %}
INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME, FLAGS {% if TitleName -%},  COL_TITLE_NAME{%- endif %} {% if ShortName -%}, COL_SHORT_NAME{%- endif %}, ICON_FILENAME, ICON_SKIN_ID) 
VALUES ( '{{Name}}', '{{GroupName}}', {% if Locked -%} 12 {% else %} 8 {%- endif %} {% if TitleName -%}, "{{TitleName}}"{%- endif %} {% if ShortName -%}, "{{ShortName}}"{%- endif %}, "{{IconFileName}}", "{{IconSkinId}}");
{% endfor %}

{% for i in range(feat_tree.CaStorageDbSetting.OperatorURLs._value|length) -%}
    {% set URLTitle = feat_tree.CaStorageDbSetting.OperatorURLs.OperatorURLTitle._value[i] %}
    {%- set URLShortTitle = feat_tree.CaStorageDbSetting.OperatorURLs.OperatorURLShortTitle._value[i] or '' -%}
    {%- set URL = feat_tree.CaStorageDbSetting.OperatorURLs.OperatorURL._value[i] or '' -%}
    {%- set URLIcon = feat_tree.CaStorageDbSetting.OperatorURLs.OperatorURLIcon.localPath._value[i] or '' -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.OperatorURLs.IconSkinId._value[i] or '' -%}
    {% if IconSkinId == "" and URLIcon != "" and URLIcon != "dummy" -%}
        {% set URLIcon = "z:\\private\\20022F35\\customsvg\\" + URLIcon %}
    {% endif %}
    INSERT INTO URL ( URL_TITLE {% if URLShortTitle -%}, URL_SHORT_TITLE{%- endif %}, URL_DEST, ICON_FILENAME, ICON_SKIN_ID )
    VALUES ('{{URLTitle}}' {% if URLShortTitle -%}, "{{URLShortTitle}}"{%- endif %}, "{{URL}}", "{{URLIcon}}", "{{IconSkinId}}" );
{% endfor %}

{% for index in range(feat_tree.CaStorageDbSetting.OperatorCollectionItems._value|length) -%}
    {% set Collection = feat_tree.CaStorageDbSetting.OperatorCollections.Name._value[0] %}
    {%- set Type = feat_tree.CaStorageDbSetting.OperatorCollectionItems.Type._value[index] or '' -%}
    {%- set Application = feat_tree.CaStorageDbSetting.OperatorCollectionItems.Application._value[index] or '' -%}
    {%- set OperatorURLTitle = feat_tree.CaStorageDbSetting.OperatorCollectionItems.OperatorURL._value[index] or '' -%}
    {%- set HomescreenWidget = feat_tree.CaStorageDbSetting.OperatorCollectionItems.HomescreenWidget._value[index] or '' -%}
{% if Type == "1" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", '', {{Application}},  '{{Collection}}' );{%- endif %}
{% if Type == "3" -%}
  {% for i in range(feat_tree.CaStorageDbSetting.OperatorURLs._value|length) if OperatorURLTitle == feat_tree.CaStorageDbSetting.OperatorURLs.OperatorURLTitle._value[i] -%}
    INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, COLLECTION_NAME)
    VALUES ( "url", '{{OperatorURLTitle}}',  '{{Collection}}' );
  {% endfor %}
{%- endif %}
{% if Type == "2" -%} INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON_FILENAME, ICON_SKIN_ID, URI, COLLECTION_NAME ) 
  VALUES ("widget", '', '', '', "{{HomescreenWidget}}", '{{Collection}}' ); {%- endif %}
{% endfor %}

{% for col in range(feat_tree.CaStorageDbSetting.Collections._value|length) -%}
    {%- set Name = feat_tree.CaStorageDbSetting.Collections.Name._value[col] or '' -%}
    {% set LocalizeNames = feat_tree.CaStorageDbSetting.Collections.LocalizeNames._value[col] or false -%}
    {%- set ShortName = feat_tree.CaStorageDbSetting.Collections.ShortName._value[col] or '' -%}
    {%- set TitleName = feat_tree.CaStorageDbSetting.Collections.TitleName._value[col] or '' -%}
    {%- set GroupName = feat_tree.CaStorageDbSetting.Collections.GroupName._value[col] or '' -%}
    {%- set Locked = feat_tree.CaStorageDbSetting.Collections.Locked._value[col] or false -%}
    {%- set TranslationFile = feat_tree.CaStorageDbSetting.Collections.TranslationFile._value[col]|string|reverse|replace('_','',1)|reverse+'_' or ''-%}
    {%- set IconFileName = feat_tree.CaStorageDbSetting.Collections.Icon.localPath._value[col] or '' -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.Collections.IconSkinId._value[col] or '' -%}
    {% if IconSkinId == "" and IconFileName != "" and IconFileName != "dummy" -%}
        {% set IconFileName = "z:\\private\\20022F35\\customsvg\\" + IconFileName %}
    {% endif %}
INSERT INTO COLLECTION (COLLECTION_NAME, COL_APP_GROUP_NAME, FLAGS {% if TitleName -%},  COL_TITLE_NAME{%- endif %}{% if LocalizeNames -%},  TRANSLATION_FILE{%- endif %}{% if ShortName -%}, COL_SHORT_NAME{%- endif %}, ICON_FILENAME, ICON_SKIN_ID) 
VALUES ( '{{Name}}', '{{GroupName}}', {% if Locked -%} 12 {% else %} 8 {%- endif %} {% if TitleName -%}, "{{TitleName}}"{%- endif %}{% if LocalizeNames -%}, "{{TranslationFile}}"{%- endif %}{% if ShortName -%}, "{{ShortName}}"{%- endif %}, "{{IconFileName}}", "{{IconSkinId}}");
{% endfor %}

{% for i in range(feat_tree.CaStorageDbSetting.URLs._value|length) -%}
    {% set URLTitle = feat_tree.CaStorageDbSetting.URLs.URLTitle._value[i] %}
    {%- set URLShortTitle = feat_tree.CaStorageDbSetting.URLs.URLShortTitle._value[i] or '' -%}
    {%- set URL = feat_tree.CaStorageDbSetting.URLs.URL._value[i] or '' -%}
    {%- set URLIcon = feat_tree.CaStorageDbSetting.URLs.URLIcon.localPath._value[i] or '' -%}
    {%- set IconSkinId = feat_tree.CaStorageDbSetting.URLs.IconSkinId._value[i] or '' -%}
    {% if IconSkinId == "" and URLIcon != "" and URLIcon != "dummy" -%}
        {% set URLIcon = "z:\\private\\20022F35\\customsvg\\" + URLIcon %}
    {% endif %}
    INSERT INTO URL ( URL_TITLE {% if URLShortTitle -%}, URL_SHORT_TITLE{%- endif %}, URL_DEST, ICON_FILENAME, ICON_SKIN_ID ) 
    VALUES ( '{{URLTitle}}' {% if URLShortTitle -%}, "{{URLShortTitle}}"{%- endif %}, "{{URL}}", "{{URLIcon}}", "{{IconSkinId}}" );
{% endfor %}

{% for index in range(feat_tree.CaStorageDbSetting.Items._value|length) -%}
    {%- set Type = feat_tree.CaStorageDbSetting.Items.Type._value[index] or '' -%}
    {%- set Application = feat_tree.CaStorageDbSetting.Items.Application._value[index] or '' -%}
    {%- set URLTitle = feat_tree.CaStorageDbSetting.Items.URL._value[index] or '' -%}
    {%- set HomescreenWidget = feat_tree.CaStorageDbSetting.Items.HomescreenWidget._value[index] or '' -%}
    {%- set Collection = feat_tree.CaStorageDbSetting.Items.ColName._value[index] or '' -%}
{% if Type == "1" -%} INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ITEM_DEST, COLLECTION_NAME) 
  VALUES ("application", '', {{Application}},  '{{Collection}}' );{%- endif %}
{% if Type == "3" -%}
  {% for i in range(feat_tree.CaStorageDbSetting.URLs._value|length) if URLTitle == feat_tree.CaStorageDbSetting.URLs.URLTitle._value[i] -%}
    INSERT INTO ITEM_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, COLLECTION_NAME)
    VALUES ( "url", '{{URLTitle}}',  '{{Collection}}' );
  {% endfor %}
{%- endif %}
{% if Type == "2" -%} INSERT INTO WIDGET_TO_COLLECTION (ITEM_TYPE, ITEM_NAME, ICON_FILENAME, ICON_SKIN_ID, URI, COLLECTION_NAME ) 
  VALUES ("widget", '{{HomescreenWidget}}', '', '', "{{HomescreenWidget}}", '{{Collection}}' ); {%- endif %}
{% endfor %}

SELECT "castoragedb_variant - END" AS " ";
SELECT "------------------------------------" AS " ";

