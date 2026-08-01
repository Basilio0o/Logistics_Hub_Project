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
