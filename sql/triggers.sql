BEGIN;

CREATE OR REPLACE FUNCTION update_product_stock()
RETURNS TRIGGER AS $$
DECLARE
    v_product_id INT;
BEGIN
    IF TG_OP IN ('INSERT', 'UPDATE') THEN
        v_product_id := NEW.product_id;
    ELSIF TG_OP = 'DELETE' THEN
        v_product_id := OLD.product_id;
    END IF;

    UPDATE products
    SET quantity_in_stock = (
        SELECT COALESCE(SUM(quantity), 0)
        FROM shelf_products
        WHERE product_id = v_product_id
    )
    WHERE id = v_product_id;
    
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_shelf_products_stock
AFTER INSERT OR UPDATE OR DELETE ON shelf_products
FOR EACH ROW
EXECUTE FUNCTION update_product_stock();

COMMIT;

CREATE OR REPLACE FUNCTION set_parcel_status_dates()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status = 'assembled' THEN
        NEW.assembled_at := CURRENT_TIMESTAMP;
    ELSIF NEW.status = 'dispatched' THEN
        NEW.dispatched_at := CURRENT_TIMESTAMP;
    ELSIF NEW.status = 'delivered' THEN
        NEW.delivered_at := CURRENT_TIMESTAMP;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_parcels_status_dates
BEFORE UPDATE OF status ON parcels
FOR EACH ROW
WHEN (OLD.status IS DISTINCT FROM NEW.status)
EXECUTE FUNCTION set_parcel_status_dates();

CREATE OR REPLACE FUNCTION log_parcel_status_change()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status IS DISTINCT FROM OLD.status THEN
    	INSERT INTO parcel_history (parcel_id, old_status, new_status, changed_by)
	VALUES (NEW.id, OLD.status, NEW.status, NULLIF(current_setting('app.current_user_id', true), '')::int);
    END IF;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_parcel_status_history
AFTER INSERT OR UPDATE OF status ON parcels
FOR EACH ROW
EXECUTE FUNCTION log_parcel_status_change();

BEGIN;

CREATE OR REPLACE FUNCTION update_parcel_dimensions()
RETURNS TRIGGER AS $$
DECLARE
    v_parcel_id INT;
BEGIN
    IF TG_OP IN ('INSERT', 'UPDATE') THEN
        v_parcel_id := NEW.parcel_id;
    ELSIF TG_OP = 'DELETE' THEN
        v_parcel_id := OLD.parcel_id;
    END IF;

    UPDATE parcels
    SET weight = (
        SELECT COALESCE(SUM(pi.quantity * p.unit_weight), 0)
        FROM parcel_items pi
        JOIN products p ON p.id = pi.product_id
        WHERE pi.parcel_id = v_parcel_id
    ),
    volume = (
        SELECT COALESCE(SUM(pi.quantity * p.unit_volume), 0)
        FROM parcel_items pi
        JOIN products p ON p.id = pi.product_id
        WHERE pi.parcel_id = v_parcel_id
    )
    WHERE id = v_parcel_id;

    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_parcel_items_dimensions ON parcel_items;
CREATE TRIGGER trg_parcel_items_dimensions
AFTER INSERT OR UPDATE OR DELETE ON parcel_items
FOR EACH ROW
EXECUTE FUNCTION update_parcel_dimensions();

COMMIT;

CREATE OR REPLACE FUNCTION fn_vehicles_load() 
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.vehicle_id IS DISTINCT FROM OLD.vehicle_id THEN
        IF OLD.vehicle_id IS NOT NULL THEN
            UPDATE vehicles
            SET current_weight  = current_weight - OLD.weight,
                current_volume  = current_volume - OLD.volume,
                current_parcels = current_parcels - 1
            WHERE id = OLD.vehicle_id;
        END IF;
        IF NEW.vehicle_id IS NOT NULL THEN
            UPDATE vehicles
            SET current_weight  = current_weight + NEW.weight,
                current_volume  = current_volume + NEW.volume,
                current_parcels = current_parcels + 1
            WHERE id = NEW.vehicle_id;
        END IF;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_vehicles_load ON parcels;
CREATE TRIGGER trg_vehicles_load
AFTER UPDATE OF vehicle_id ON parcels
FOR EACH ROW EXECUTE FUNCTION fn_vehicles_load();

CREATE OR REPLACE FUNCTION fn_vehicles_status_change() RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status = 'on_route' AND OLD.status <> 'on_route' THEN
        -- отправка в рейс: посылки машины становятся dispatched
        UPDATE parcels
        SET status = 'dispatched'
        WHERE vehicle_id = NEW.id AND status = 'loaded';
    ELSIF OLD.status = 'on_route' AND NEW.status = 'available' THEN
        -- возврат из рейса: посылки доставлены, машина разгружена
        UPDATE parcels
        SET status = 'delivered', vehicle_id = NULL
        WHERE vehicle_id = NEW.id AND status IN ('loaded', 'dispatched');
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_vehicles_status_change ON vehicles;
CREATE TRIGGER trg_vehicles_status_change
AFTER UPDATE OF status ON vehicles
FOR EACH ROW EXECUTE FUNCTION fn_vehicles_status_change();
