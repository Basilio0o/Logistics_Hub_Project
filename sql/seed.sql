INSERT INTO users (login, pass_hash, role) VALUES
    ('admin', 'admin123', 'admin'),
    ('receiver1', 'recv123', 'receiver')
ON CONFLICT (login) DO NOTHING;

INSERT INTO districts (name) VALUES
    ('Центральный'),
    ('Северный')
ON CONFLICT (name) DO NOTHING;

INSERT INTO suppliers (name, phone, address) VALUES
    ('ООО Поставщик', '+79991234567', 'г. Москва, ул. Ленина, 1'),
    ('ИП Иванов', '+79997654321', 'г. Москва, ул. Мира, 5')
ON CONFLICT DO NOTHING;

INSERT INTO warehouses (name, address, square) VALUES
    ('Главный склад', 'г. Москва, Промзона, 10', 5000.00)
ON CONFLICT DO NOTHING;

INSERT INTO zones (warehouse_id, name, code, max_weight, max_volume) VALUES
    (1, 'Обычная', 'A', 5000.00, 2500.00),
    (1, 'Холодная', 'B', 2000.00, 1000.00),
    (1, 'Крупногабаритная', 'C', 10000.00, 5000.00)
ON CONFLICT DO NOTHING;

INSERT INTO racks (zone_id, code, max_weight, max_volume) VALUES
    (1, 'A-01', 1000.00, 500.00),
    (2, 'B-01', 500.00,  250.00),
    (3, 'C-01', 2000.00, 1000.00)
ON CONFLICT DO NOTHING;

INSERT INTO shelves (rack_id, code, max_weight, max_volume, status) VALUES
    (1, 'A-01-01', 500.00, 250.00, 'active'),
    (1, 'A-01-02', 500.00, 250.00, 'active'),
    (2, 'B-01-01', 250.00, 125.00, 'active'),
    (2, 'B-01-02', 250.00, 125.00, 'maintenance'),  -- одна на обслуживании для теста
    (3, 'C-01-01', 1000.00, 500.00, 'active'),
    (3, 'C-01-02', 1000.00, 500.00, 'blocked')      -- одна заблокирована для теста
ON CONFLICT DO NOTHING;

INSERT INTO products (name, supplier_id, type, unit_weight, unit_volume, unit_price, shelf_life_days) VALUES
    ('Книга',        1, 'regular',    0.50,  0.002, 500.00, NULL),
    ('Молоко',       1, 'perishable', 1.00,  0.001, 80.00,  7),
    ('Велосипед',    1, 'oversized', 15.00,  0.500, 15000.00, NULL)
ON CONFLICT DO NOTHING;

INSERT INTO shelf_products (shelf_id, product_id, quantity, received_at) VALUES
    (1, 1, 100, CURRENT_TIMESTAMP - INTERVAL '5 days'),   -- книги на полке A-01-01, лежат 5 дней
    (2, 1, 50,  CURRENT_TIMESTAMP - INTERVAL '2 days'),   -- книги на полке A-01-02, лежат 2 дня
    (3, 2, 30,  CURRENT_TIMESTAMP - INTERVAL '3 days'),   -- молоко на полке B-01-01, лежат 3 дня
    (4, 2, 20,  CURRENT_TIMESTAMP - INTERVAL '1 day'),    -- молоко на полке B-01-02, лежит 1 день
    (5, 3, 5,   CURRENT_TIMESTAMP)                        -- велосипеды на полке C-01-01
ON CONFLICT (shelf_id, product_id) DO NOTHING;

INSERT INTO supplies (supplier_id, received_by, received_at, status) VALUES
    (1, NULL, NULL, 'pending'),                                 -- поставка 1: ожидает приёмки
    (2, 2, CURRENT_TIMESTAMP - INTERVAL '1 day', 'accepted')    -- поставка 2: принята вчера
ON CONFLICT DO NOTHING;

INSERT INTO supply_items (supply_id, product_id, quantity, received_at) VALUES
    (1, 1, 50, CURRENT_TIMESTAMP),                      -- 50 книг в поставке 1
    (1, 2, 20, CURRENT_TIMESTAMP),                      -- 20 единиц молока в поставке 1
    (2, 3, 10, CURRENT_TIMESTAMP - INTERVAL '1 day')    -- 10 велосипедов в поставке 2
ON CONFLICT (supply_id, product_id) DO NOTHING;

INSERT INTO orders (district_id, priority, status) VALUES
    (1, 'normal', 'new'),
    (1, 'high',   'new')
ON CONFLICT DO NOTHING;

INSERT INTO order_items (order_id, product_id, quantity) VALUES
    (1, 1, 2),   -- 2 книги в заказе 1
    (1, 2, 1),   -- 1 молоко в заказе 1
    (2, 3, 1)    -- 1 велосипед в заказе 2
ON CONFLICT (order_id, product_id) DO NOTHING;

INSERT INTO vehicles (number, driver, max_weight, max_volume, max_parcels) VALUES
    ('А123БВ77', 'Петров П.П.', 500.00, 10.00, 20),
    ('Х789ОР77', 'Сидоров С.С.', 1000.00, 20.00, 30)
ON CONFLICT (number) DO NOTHING;

INSERT INTO vehicle_districts (vehicle_id, district_id) VALUES
    (1, 1),   -- машина 1 обслуживает Центральный
    (1, 2),   -- машина 1 обслуживает Северный
    (2, 1)    -- машина 2 обслуживает Центральный
ON CONFLICT (vehicle_id, district_id) DO NOTHING;

INSERT INTO parcels (order_id, district_id, type, weight, volume, priority, status) VALUES
    (1, 1, 'standard', 2.00, 0.005, 'normal', 'new')
ON CONFLICT DO NOTHING;

INSERT INTO parcel_items (parcel_id, product_id, quantity) VALUES
    (1, 1, 2),   -- 2 книги
    (1, 2, 1)    -- 1 молоко
ON CONFLICT (parcel_id, product_id) DO NOTHING;

INSERT INTO audit_logs (entity_type, entity_id, operation, performed_by, details) VALUES
    ('order', 1, 'receive', 1, '{"note": "Заказ создан менеджером"}'::jsonb)
ON CONFLICT DO NOTHING;
