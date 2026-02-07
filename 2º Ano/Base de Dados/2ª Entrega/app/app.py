import os
from logging.config import dictConfig

from flask import Flask, jsonify, request
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from psycopg.rows import namedtuple_row
from psycopg_pool import ConnectionPool
import psycopg.errors

dictConfig(
    {
        "version": 1,
        "formatters": {
            "default": {
                "format": "[%(asctime)s] %(levelname)s in %(module)s:%(lineno)s - %(funcName)20s(): %(message)s",
            }
        },
        "handlers": {
            "wsgi": {
                "class": "logging.StreamHandler",
                "stream": "ext://flask.logging.wsgi_errors_stream",
                "formatter": "default",
            }
        },
        "root": {"level": "INFO", "handlers": ["wsgi"]},
    }
)

RATELIMIT_STORAGE_URI = os.environ.get("RATELIMIT_STORAGE_URI", "memory://")

app = Flask(__name__)
app.config.from_prefixed_env()
log = app.logger
limiter = Limiter(
    get_remote_address,
    app=app,
    default_limits=["200 per day", "50 per hour"],
    storage_uri=RATELIMIT_STORAGE_URI,
)

# Use the DATABASE_URL environment variable if it exists, otherwise use the default.
# Use the format postgres://aviacao:aviacao@postgres/aviacao to connect to the database.
DATABASE_URL = os.environ.get("DATABASE_URL", "postgres://aviacao:aviacao@postgres/aviacao")

pool = ConnectionPool(
    conninfo=DATABASE_URL,
    kwargs={
        "autocommit": True,  # If True don’t start transactions automatically.
        "row_factory": namedtuple_row,
    },
    min_size=4,
    max_size=10,
    open=True,
    # check=ConnectionPool.check_connection,
    name="postgres_pool",
    timeout=5,
)

@app.route("/", methods=("GET",))
@limiter.limit("1 per second")
def airport_index():
  """Show all airports: name + city"""
  with pool.connection() as conn:
    with conn.cursor() as cur:
      airports = cur.execute(
        """
        SELECT nome, cidade
        FROM aeroporto
        """
      ).fetchall()
      log.debug(f"Found {cur.rowcount} rows.")
      
  return jsonify(airports), 200


@app.route("/voos/<partida>/", methods=("GET",))
def flight_from_departure(partida):
  """Get all flights which depart from a given airport"""
  with pool.connection() as conn:
    with conn.cursor() as cur:
      flights = cur.execute(
        """
        SELECT no_serie, hora_partida, chegada
        FROM voo v JOIN aeroporto a ON (v.partida = a.codigo)
        WHERE v.partida = %(partida)s
                  AND v.hora_partida > CURRENT_TIMESTAMP
                  AND v.hora_partida <= CURRENT_TIMESTAMP + INTERVAL '12 hours'
                ORDER BY v.hora_partida
                """,
        {"partida": partida},
      ).fetchall()
      log.debug(f'Found {cur.rowcount} rows.')
      
  if flights is None:
    return jsonify({"message": "No flights departing from airport {partida} were found.", "status": "error"}), 404
  
  return jsonify(flights), 200
    
@app.route("/voos/<partida>/<chegada>/", methods=("GET",))
def next_flights(partida, chegada):
    with pool.connection() as conn:
        with conn.cursor() as cur:
            flights = cur.execute(
                """
                SELECT v.no_serie, v.hora_partida
                FROM voo v
                WHERE v.partida = %(partida)s
                  AND v.chegada = %(chegada)s
                  AND v.hora_partida > CURRENT_TIMESTAMP
                  AND (
                    SELECT COUNT(*)
                    FROM bilhete b
                    WHERE b.voo_id = v.id
                  ) < (
                    SELECT COUNT(*)
                    FROM assento a
                    WHERE a.no_serie = v.no_serie
                  )
                ORDER BY v.hora_partida
                LIMIT 3
                """,
                {"partida": partida, "chegada": chegada}
            ).fetchall()
            log.debug(f'Found {cur.rowcount} rows.')
    
    if not flights:  # Corrigido: verifica lista vazia
        return jsonify({
            "message": f"No flights from {partida} to {chegada} with available seats were found.", 
            "status": "error"
        }), 404
    
    return jsonify(flights), 200


@app.route("/compra/<voo>/", methods=("POST",))
def register_purchase(voo):
  """Populates the DB with a purchase and its newly bought tickets."""
  data = request.get_json()
  nif_cliente = data.get("nif_cliente")
  dados_bilhetes = data.get('dados_bilhetes')
  
  if not nif_cliente:
    return jsonify({"message": "Missing required parameters: nif_cliente.", "status": "error"}), 400
  
  if not dados_bilhetes:
    return jsonify({"message": "Missing required parameters: dados_bilhetes.", "status": "error"}), 400
  
  dados_bilhetes = [tuple(items) for items in dados_bilhetes]
  
  with pool.connection() as conn:
    with conn.cursor() as cur:
      # Insert purchase
      cur.execute("""
                  INSERT INTO venda (nif_cliente, balcao, hora)
                  VALUES (%(nif_cliente)s, NULL, CURRENT_TIMESTAMP)
                  RETURNING codigo_reserva
                  """,
                  {"nif_cliente": nif_cliente})
      codigo_reserva_venda = cur.fetchone().codigo_reserva
      
      no_serie_aviao = cur.execute("""
                                   SELECT no_serie FROM voo
                                   WHERE id = %(voo)s
                                   """, {"voo": voo}).fetchone().no_serie
      for nome_passageiro, prim_classe in dados_bilhetes:
        prim_classe = 'TRUE' if prim_classe else 'FALSE'
        preco = 550.0 * 1 if prim_classe == 'FALSE' else 550.0 * 1.5
        preco = str(preco)
        cur.execute("""
                    INSERT INTO bilhete (voo_id, codigo_reserva, nome_passegeiro, preco, prim_classe, lugar, no_serie)
                    VALUES (%(voo)s, %(codigo_reserva)s, %(nome_passageiro)s, %(preco)s, %(prim_classe)s, NULL, %(no_serie)s)
                    """,
                    {"voo": voo,
                     "codigo_reserva": codigo_reserva_venda,
                     "nome_passageiro": nome_passageiro,
                     "preco": preco,
                     "prim_classe": prim_classe,
                     "no_serie": no_serie_aviao})
        
      return jsonify({"message": "Purchase registered successfully.", "status": "success"}), 201

@app.route("/checkin/<bilhete>/", methods=("PUT", "POST"))
def checkin(bilhete):
    """Check in a passenger for a flight."""
    with pool.connection() as conn:
        with conn.cursor() as cur:
          bilhete_info = cur.execute(
                """
                SELECT no_serie, prim_classe, lugar, voo_id FROM bilhete
                WHERE id = %(bilhete)s;
                """,
                {"bilhete": bilhete}
            ).fetchone()
            
          assento = cur.execute(
                """
                SELECT lugar FROM assento
                WHERE no_serie = %(no_serie)s
                  AND prim_classe = %(classe)s
                  AND lugar NOT IN (
                      SELECT lugar FROM bilhete
                      WHERE lugar IS NOT NULL
                        AND no_serie = %(no_serie)s
                        AND voo_id = %(v_id)s
                  )
                LIMIT 1;
                """, 
                {
                    "no_serie": bilhete_info.no_serie,
                    "classe": bilhete_info.prim_classe,
                    "bilhete": bilhete,
                    "v_id": bilhete_info.voo_id
                }
            ).fetchone().lugar
                    
          cur.execute(
              """
              UPDATE bilhete
              SET lugar = %(assento)s
              WHERE id = %(bilhete)s;
              """,
              {"assento": assento, "bilhete": bilhete},
          )
    return jsonify({"message": "Check-in successful.", "status": "success"}), 200