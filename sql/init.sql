CREATE TABLE IF NOT EXISTS users (
    id         SERIAL PRIMARY KEY,
    login      VARCHAR(50) UNIQUE NOT NULL,
    pass_hash  VARCHAR(255) NOT NULL,
    role       VARCHAR(20) NOT NULL 
               CHECK (role IN ('receiver','assembler','manager','admin')),
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS districts (
    id   SERIAL PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS suppliers (
    id      SERIAL PRIMARY KEY,
    name    VARCHAR(255) NOT NULL,
    phone   VARCHAR(20),
    address TEXT
);

CREATE TABLE IF NOT EXISTS warehouses (
    id      SERIAL PRIMARY KEY,
    name    VARCHAR(100) NOT NULL,
    address TEXT NOT NULL,
    square  NUMERIC(10,2) NOT NULL
);
