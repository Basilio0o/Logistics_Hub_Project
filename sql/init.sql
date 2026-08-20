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
                   CHECK (name IN ('regular', 'cold', 'oversized')),
    code           VARCHAR(10) NOT NULL,
    max_weight     NUMERIC(10,3),
    max_volume     NUMERIC(10,3),
    current_weight NUMERIC(10,3) DEFAULT 0,
    current_volume NUMERIC(10,3) DEFAULT 0
);

CREATE TABLE IF NOT EXISTS racks (
    id             SERIAL PRIMARY KEY,
    zone_id        INT NOT NULL REFERENCES zones(id),
    code           VARCHAR(20) NOT NULL,
    max_weight     NUMERIC(10,3),
    max_volume     NUMERIC(10,3),
    current_weight NUMERIC(10,3) DEFAULT 0,
    current_volume NUMERIC(10,3) DEFAULT 0
);

CREATE TABLE IF NOT EXISTS shelves (
    id             SERIAL PRIMARY KEY,
    rack_id        INT NOT NULL REFERENCES racks(id),
    code           VARCHAR(30) NOT NULL,
    max_weight     NUMERIC(10,3),
    max_volume     NUMERIC(10,3),
    current_weight NUMERIC(10,3) DEFAULT 0,
    current_volume NUMERIC(10,3) DEFAULT 0,
    status         VARCHAR(20) DEFAULT 'active'
                   CHECK (status IN ('active', 'blocked', 'maintenance'))
);

CREATE TABLE IF NOT EXISTS products (
    id                SERIAL PRIMARY KEY,
    name              VARCHAR(255) NOT NULL,
    supplier_id       INT REFERENCES suppliers(id),
    type              VARCHAR(20) NOT NULL
                      CHECK (type IN ('regular', 'perishable', 'oversized')),
    unit_weight       NUMERIC(10,3) NOT NULL,
    unit_volume       NUMERIC(10,3) NOT NULL,
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

CREATE TABLE IF NOT EXISTS supplies (
    id          SERIAL PRIMARY KEY,
    supplier_id INT NOT NULL REFERENCES suppliers(id),
    received_by INT REFERENCES users(id),
    received_at TIMESTAMPTZ,
    status      VARCHAR(20) DEFAULT 'pending'
                CHECK (status IN ('pending', 'accepted', 'rejected'))
);

CREATE TABLE IF NOT EXISTS supply_items (
    supply_id   INT NOT NULL REFERENCES supplies(id),
    product_id  INT NOT NULL REFERENCES products(id),
    quantity    INT NOT NULL,
    received_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (supply_id, product_id)
);

CREATE TABLE IF NOT EXISTS orders (
    id          SERIAL PRIMARY KEY,
    district_id INT NOT NULL REFERENCES districts(id),
    priority    VARCHAR(10) DEFAULT 'normal'
                CHECK (priority IN ('low', 'normal', 'high', 'urgent')),
    status      VARCHAR(20) DEFAULT 'new'
                CHECK (status IN ('new', 'processing', 'assembled',
                                  'dispatched', 'delivered', 'canceled')),
    created_at  TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS order_items (
    order_id   INT NOT NULL REFERENCES orders(id),
    product_id INT NOT NULL REFERENCES products(id),
    quantity   INT NOT NULL,
    PRIMARY KEY (order_id, product_id)
);

CREATE TABLE IF NOT EXISTS vehicles (
    id              SERIAL PRIMARY KEY,
    number          VARCHAR(20) UNIQUE NOT NULL,
    driver          VARCHAR(100) NOT NULL,
    max_weight      NUMERIC(10,3) NOT NULL,
    max_volume      NUMERIC(10,3) NOT NULL,
    max_parcels     INT NOT NULL,
    current_weight  NUMERIC(10,3) DEFAULT 0,
    current_volume  NUMERIC(10,3) DEFAULT 0,
    current_parcels INT DEFAULT 0,
    status          VARCHAR(20) DEFAULT 'available'
                    CHECK (status IN ('available', 'loading', 'on_route', 'off_duty')),
    dispatched_by   INT REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS vehicle_districts (
    vehicle_id  INT NOT NULL REFERENCES vehicles(id),
    district_id INT NOT NULL REFERENCES districts(id),
    PRIMARY KEY (vehicle_id, district_id)
);

CREATE TABLE IF NOT EXISTS parcels (
    id            SERIAL PRIMARY KEY,
    order_id      INT NOT NULL REFERENCES orders(id),
    district_id   INT NOT NULL REFERENCES districts(id),
    type          VARCHAR(20) NOT NULL
                  CHECK (type IN ('standard', 'express', 'oversized')),
    weight        NUMERIC(10,3) NOT NULL,
    volume        NUMERIC(10,3) NOT NULL,
    priority      VARCHAR(10) NOT NULL
                  CHECK (priority IN ('low', 'normal', 'high', 'urgent')),
    status        VARCHAR(20) DEFAULT 'new'
                  CHECK (status IN ('new', 'assembled', 'grouped',
                                    'loaded', 'dispatched', 'delivered')),
    assembled_by  INT REFERENCES users(id),
    created_at    TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    assembled_at  TIMESTAMPTZ,
    dispatched_at TIMESTAMPTZ,
    delivered_at  TIMESTAMPTZ,
    vehicle_id    INT REFERENCES vehicles(id)
);

CREATE TABLE IF NOT EXISTS parcel_items (
    parcel_id  INT NOT NULL REFERENCES parcels(id),
    product_id INT NOT NULL REFERENCES products(id),
    quantity   INT NOT NULL,
    PRIMARY KEY (parcel_id, product_id)
);

CREATE TABLE IF NOT EXISTS parcel_history (
    id         SERIAL PRIMARY KEY,
    parcel_id  INT NOT NULL REFERENCES parcels(id),
    old_status VARCHAR(20)
               CHECK (old_status IS NULL OR
                      old_status IN ('new', 'assembled', 'grouped',
                                     'loaded', 'dispatched', 'delivered')),
    new_status VARCHAR(20) NOT NULL
               CHECK (new_status IN ('new', 'assembled', 'grouped',
                                     'loaded', 'dispatched', 'delivered')),
    changed_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    changed_by INT REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS audit_logs (
    id           SERIAL PRIMARY KEY,
    entity_type  VARCHAR(50) NOT NULL
                 CHECK (entity_type IN ('shipment', 'supplier', 'order', 'parcel', 'item', 'cell', 'vehicle', 'district')),
    entity_id    INT NOT NULL,
    operation    VARCHAR(50) NOT NULL
                 CHECK (operation IN ('receive', 'assemble', 'dispatch',
                                      'move', 'update', 'delete')),
    performed_by INT REFERENCES users(id),
    created_at   TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    details      JSONB
);