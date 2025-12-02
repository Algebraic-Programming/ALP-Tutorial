# -------------------------------
# CONFIGURATION
# -------------------------------
BASE_ENV="py311"
WHEEL_DIR="$HOME/download"
SCRIPT_PATH="$HOME/download/GitHub/ALP-Tutorial/exercises/ex9/exercise9_starter.py"
PKG_NAME="alp_graphblas"

# -------------------------------
# CREATE UNIQUE TEMP ENV NAME
# -------------------------------
RAND_SUFFIX=$(tr -dc 'a-z0-9' < /dev/urandom | head -c 6)
TMP_ENV="tmp_env_${RAND_SUFFIX}"

echo ">>> Creating temporary environment: $TMP_ENV"

# -------------------------------
# CLONE BASE ENV
# -------------------------------
conda create --name "$TMP_ENV" --clone "$BASE_ENV" -y

# -------------------------------
# ACTIVATE TEMP ENV AND INSTALL WHEEL
# -------------------------------
source "$(conda info --base)/etc/profile.d/conda.sh"
conda activate "$TMP_ENV"

echo ">>> Installing wheel $PKG_NAME from $WHEEL_DIR"
pip install --no-index --find-links "$WHEEL_DIR" "$PKG_NAME"

# -------------------------------
# RUN SCRIPT
# -------------------------------
echo ">>> Running script: $SCRIPT_PATH"
python3 "$SCRIPT_PATH"

# -------------------------------
# CLEAN UP
# -------------------------------
echo ">>> Deactivating and deleting temporary environment: $TMP_ENV"
conda deactivate
conda remove -y --name "$TMP_ENV" --all

echo ">>> Done."
