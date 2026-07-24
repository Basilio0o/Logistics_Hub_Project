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
