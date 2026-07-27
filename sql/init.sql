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

CREATE TABLE IF NOT EXISTS zones (
    id             SERIAL PRIMARY KEY,
    warehouse_id   INT NOT NULL REFERENCES warehouses(id),
    name           VARCHAR(50) NOT NULL
                   CHECK (name IN ('Обычная', 'Холодная', 'Крупногабаритная')),
    code           VARCHAR(10) NOT NULL,
    max_weight     NUMERIC(10,2),
    max_volume     NUMERIC(10,2),
    current_weight NUMERIC(10,2) DEFAULT 0,
    current_volume NUMERIC(10,2) DEFAULT 0
);

CREATE TABLE IF NOT EXISTS racks (
    id             SERIAL PRIMARY KEY,
    zone_id        INT NOT NULL REFERENCES zones(id),
    code           VARCHAR(20) NOT NULL,
    max_weight     NUMERIC(10,2),
    max_volume     NUMERIC(10,2),
    current_weight NUMERIC(10,2) DEFAULT 0,
    current_volume NUMERIC(10,2) DEFAULT 0
);

CREATE TABLE IF NOT EXISTS shelves (
    id             SERIAL PRIMARY KEY,
    rack_id        INT NOT NULL REFERENCES racks(id),
    code           VARCHAR(30) NOT NULL,
    max_weight     NUMERIC(10,2),
    max_volume     NUMERIC(10,2),
    current_weight NUMERIC(10,2) DEFAULT 0,
    current_volume NUMERIC(10,2) DEFAULT 0,
    status         VARCHAR(20) DEFAULT 'active'
                   CHECK (status IN ('active', 'blocked', 'maintenance'))
);

CREATE TABLE IF NOT EXISTS products (
    id                SERIAL PRIMARY KEY,
    name              VARCHAR(255) NOT NULL,
    supplier_id       INT REFERENCES suppliers(id),
    type              VARCHAR(20) NOT NULL
                      CHECK (type IN ('regular', 'perishable', 'oversized')),
    unit_weight       NUMERIC(10,2) NOT NULL,
    unit_volume       NUMERIC(10,2) NOT NULL,
    unit_price        NUMERIC(10,2) NOT NULL,
    quantity_in_stock INT DEFAULT 0,
    shelf_life_days   INT
);

CREATE TABLE IF NOT EXISTS shelf_products (
    shelf_id   INT NOT NULL REFERENCES shelves(id),
    product_id INT NOT NULL REFERENCES products(id),
    quantity   INT NOT NULL DEFAULT 0,
    received_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (shelf_id, product_id)
);