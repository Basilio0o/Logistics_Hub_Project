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
