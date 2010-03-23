@echo off
del castoragedb
sqlite3 castoragedb ".read castoragedb_create.sql"
sqlite3 castoragedb ".genfkey --exec"
sqlite3 castoragedb ".read castoragedb_variant_test.sql"
