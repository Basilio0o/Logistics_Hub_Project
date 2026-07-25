INSERT INTO users (login, pass_hash, role) VALUES
    ('admin', 'admin123', 'admin'),
    ('receiver1', 'recv123', 'receiver');

INSERT INTO districts (name) VALUES
    ('Центральный'),
    ('Северный');

INSERT INTO suppliers (name, phone, address) VALUES
    ('ООО Поставщик', '+79991234567', 'г. Москва, ул. Ленина, 1'),
    ('ИП Иванов', '+79997654321', 'г. Москва, ул. Мира, 5');

INSERT INTO warehouses (name, address, square) VALUES
    ('Главный склад', 'г. Москва, Промзона, 10', 5000.00);

INSERT INTO zones (warehouse_id, name, code, max_weight, max_volume) VALUES
    (1, 'Обычная',        'A', 5000.00, 2500.00),
    (1, 'Холодная',       'B', 2000.00, 1000.00),
    (1, 'Крупногабаритная', 'C', 10000.00, 5000.00);


INSERT INTO racks (zone_id, code, max_weight, max_volume) VALUES
    (1, 'A-01', 1000.00, 500.00),
    (2, 'B-01', 500.00,  250.00),
    (3, 'C-01', 2000.00, 1000.00);


INSERT INTO shelves (rack_id, code, max_weight, max_volume, status) VALUES
    (1, 'A-01-01', 500.00, 250.00, 'active'),
    (1, 'A-01-02', 500.00, 250.00, 'active'),
    (2, 'B-01-01', 250.00, 125.00, 'active'),
    (2, 'B-01-02', 250.00, 125.00, 'maintenance'),  -- одна на обслуживании для теста
    (3, 'C-01-01', 1000.00, 500.00, 'active'),
    (3, 'C-01-02', 1000.00, 500.00, 'blocked');     -- одна заблокирована для теста