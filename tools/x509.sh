
function generate_root_ca()
{
  local root_ca_base_name=${1}
  local common_name=${1}

  echo "Creating ${root_ca_base_name}.key.pem"

  openssl req -config openssl.cnf \
    -newkey ec:<(openssl ecparam -name prime256v1) -nodes -new \
    -subj "/CN=${common_name}" -sha256 \
    -keyout ${root_ca_base_name}.key.pem -noout

  echo "Signing ${root_ca_base_name}.cert.pem"

  openssl req -new -x509 -config openssl.cnf -extensions v3_ca \
    -subj "/CN=${common_name}" -days 3650 -sha256 \
    -key ${root_ca_base_name}.key.pem -out ${root_ca_base_name}.cert.pem

  cp ${root_ca_base_name}.cert.pem ${root_ca_base_name}.chain.cert.pem
}

function generate_intermediate_ca()
{
  local parent_ca_base_name=${1}
  local intermediate_ca_base_name=${2}
  local common_name=${2}

  openssl req -config openssl.cnf \
    -newkey ec:<(openssl ecparam -name prime256v1) -nodes \
    -subj "/CN=${common_name}" -new -sha256 \
    -keyout ${intermediate_ca_base_name}.key.pem -out ${intermediate_ca_base_name}.csr.pem

  openssl x509 -req -extfile openssl.cnf -extensions v3_intermediate_ca \
    -days 3650 -CAcreateserial -sha256 \
    -CA ${parent_ca_base_name}.cert.pem -CAkey ${parent_ca_base_name}.key.pem \
    -in ${intermediate_ca_base_name}.csr.pem -out ${intermediate_ca_base_name}.cert.pem

  rm -f ${intermediate_ca_base_name}.csr.pem

  openssl verify -CAfile ${parent_ca_base_name}.chain.cert.pem \
    ${intermediate_ca_base_name}.cert.pem

  cat ${intermediate_ca_base_name}.cert.pem \
    ${parent_ca_base_name}.chain.cert.pem >${intermediate_ca_base_name}.chain.cert.pem
}

function generate_leaf_cert()
{
  local parent_ca_base_name=${1}
  local device_base_name=${2}.device
  local common_name=${2}

  if [ $# -eq 3 ]; then
    device_base_name=${2}
    common_name=${3}
  fi

  openssl req -config openssl.cnf \
    -newkey ec:<(openssl ecparam -name prime256v1) -nodes \
    -subj "/CN=${common_name}" -new -sha256 \
    -keyout ${device_base_name}.pkkey.pem -out ${device_base_name}.csr.pem

  openssl x509 -req -extfile openssl.cnf -extensions device_cert \
    -days 3650 -CAcreateserial -sha256 \
    -CA ${parent_ca_base_name}.cert.pem -CAkey ${parent_ca_base_name}.key.pem \
    -in ${device_base_name}.csr.pem -out ${device_base_name}.cert.pem

  rm -f ${device_base_name}.csr.pem

  openssl verify -CAfile ${parent_ca_base_name}.chain.cert.pem ${device_base_name}.cert.pem

  if [ $# -eq 2 ]; then

    openssl ec -in ${device_base_name}.pkkey.pem -out ${device_base_name}.key.pem

    cat ${device_base_name}.cert.pem \
      ${parent_ca_base_name}.chain.cert.pem > ${device_base_name}.chain.cert.pem

  fi

  rm -f ${device_base_name}.pkkey.pem
}

usage()
{
  echo "Usage: createCA ca_base_name common_name"
  echo "       createICA parent_ca_base_name ica_base_name common_name"
  echo "       createDevice parent_ca_base_name device_name"
  echo "       verify ca_base_name verification_code"
  echo "       dump filename"
}

if [ "${1}" == "createCA" ]; then
  if [ $# -ne 2 ]; then
    usage
    exit 1
  fi

  generate_root_ca ${2}

elif [ "${1}" == "createICA" ]; then
  if [ $# -ne 3 ]; then
    usage
    exit 1
  fi

  generate_intermediate_ca ${2} ${3}

elif [ "${1}" == "createDevice" ]; then
  if [ $# -ne 3 ]; then
    usage
    exit 1
  fi

  generate_leaf_cert ${2} ${3}

elif [ "${1}" == "verify" ]; then
  if [ $# -ne 3 ]; then
    echo $#
    usage
    exit 1
  fi

  generate_leaf_cert ${2} "verification" ${3}

elif [ "${1}" == "dump" ]; then
  if [ $# -ne 2 ]; then
    usage
    exit 1
  fi

  openssl x509 -in ${2} -text -noout
else

  usage
  exit 1

fi
