#
# PgSQL - https://www.cybertec-postgresql.com/en/easy-guide-to-writing-postgresql-extensions/
#
EXTENSION = verhoeff
PGFILEDESC = "C Extension to compute and validate Verhoeff checksums"
MODULE_big = verhoeff
DATA = verhoeff--1.0.sql
OBJS = verhoeff.o 

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


