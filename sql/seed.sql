INSERT INTO users (login, pass_hash, role) VALUES
    ('admin_test', 'admin_test_1234', 'admin'),
    ('receiver_test', 'receiver_test_1234', 'receiver'),
    ('assembler_test', 'assembler_test_1234', 'assembler'),
    ('manager_test', 'manager_test_1234', 'manager')
ON CONFLICT (login) DO NOTHING;

INSERT INTO districts (name) VALUES
    ('Central'),
    ('North')
ON CONFLICT (name) DO NOTHING;

INSERT INTO suppliers (name, phone, address) VALUES
    ('Supplier LLC', '+79991234567', 'Moscow, Lenina St, 1'),
    ('Ivanov IE', '+79997654321', 'Moscow, Mira St, 5');

INSERT INTO warehouses (name, address, square) VALUES
    ('Main Warehouse', 'Moscow, Industrial Zone, 10', 5000.00);

INSERT INTO zones (warehouse_id, name, code, max_weight, max_volume) VALUES
    (1, 'regular', 'A', 5000.00, 2500.00),
    (1, 'cold', 'B', 2000.00, 1000.00),
    (1, 'oversized', 'C', 10000.00, 5000.00);

INSERT INTO racks (zone_id, code, max_weight, max_volume) VALUES
    (1, 'A-01', 1000.00, 500.00),
    (2, 'B-01', 500.00,  250.00),
    (3, 'C-01', 2000.00, 1000.00);

INSERT INTO shelves (rack_id, code, max_weight, max_volume, status) VALUES
    (1, 'A-01-01', 500.00, 250.00, 'active'),
    (1, 'A-01-02', 500.00, 250.00, 'active'),
    (2, 'B-01-01', 250.00, 125.00, 'active'),
    (2, 'B-01-02', 250.00, 125.00, 'maintenance'),
    (3, 'C-01-01', 1000.00, 500.00, 'active'),
    (3, 'C-01-02', 1000.00, 500.00, 'blocked');

INSERT INTO products (name, supplier_id, type, unit_weight, unit_volume, unit_price, shelf_life_days) VALUES
    ('Book',        1, 'regular',    0.50,  0.002, 500.00, NULL),
    ('Milk',        1, 'perishable', 1.00,  0.001, 80.00,  7),
    ('Bicycle',     1, 'oversized', 15.00,  0.500, 15000.00, NULL);

INSERT INTO shelf_products (shelf_id, product_id, quantity, received_at) VALUES
    (1, 1, 100, CURRENT_TIMESTAMP - INTERVAL '5 days'),
    (2, 1, 50,  CURRENT_TIMESTAMP - INTERVAL '2 days'),
    (3, 2, 30,  CURRENT_TIMESTAMP - INTERVAL '3 days'),
    (4, 2, 20,  CURRENT_TIMESTAMP - INTERVAL '1 day'),
    (5, 3, 5,   CURRENT_TIMESTAMP)
ON CONFLICT (shelf_id, product_id) DO NOTHING;

INSERT INTO supplies (supplier_id, received_by, received_at, status) VALUES
    (1, NULL, NULL, 'pending'),
    (2, 2, CURRENT_TIMESTAMP - INTERVAL '1 day', 'accepted');

INSERT INTO supply_items (supply_id, product_id, quantity, received_at) VALUES
    (1, 1, 50, CURRENT_TIMESTAMP),
    (1, 2, 20, CURRENT_TIMESTAMP),
    (2, 3, 10, CURRENT_TIMESTAMP - INTERVAL '1 day')
ON CONFLICT (supply_id, product_id) DO NOTHING;

INSERT INTO orders (district_id, priority, status) VALUES
    (1, 'normal', 'new'),
    (1, 'high',   'new');

INSERT INTO order_items (order_id, product_id, quantity) VALUES
    (1, 1, 2),
    (1, 2, 1),
    (2, 3, 1)
ON CONFLICT (order_id, product_id) DO NOTHING;

INSERT INTO vehicles (number, driver, max_weight, max_volume, max_parcels) VALUES
    ('A123BV77', 'Petrov P.P.', 500.00, 10.00, 20),
    ('X789OR77', 'Sidorov S.S.', 1000.00, 20.00, 30)
ON CONFLICT (number) DO NOTHING;

INSERT INTO vehicle_districts (vehicle_id, district_id) VALUES
    (1, 1),
    (1, 2),
    (2, 1)
ON CONFLICT (vehicle_id, district_id) DO NOTHING;

INSERT INTO parcels (order_id, district_id, type, weight, volume, priority, status) VALUES
    (1, 1, 'standard', 2.00, 0.005, 'normal', 'new');

INSERT INTO parcel_items (parcel_id, product_id, quantity) VALUES
    (1, 1, 2),
    (1, 2, 1)
ON CONFLICT (parcel_id, product_id) DO NOTHING;

INSERT INTO audit_logs (entity_type, entity_id, operation, performed_by, details) VALUES
    ('order', 1, 'receive', 1, '{"note": "Order created by manager"}'::jsonb);