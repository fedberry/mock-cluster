<?php
/**
 * @file
 * Mock Cluster REST API.
 */
 
/**
 * Return POST or PUT JSON descoded content.
 */
function mock_cluster_rest_request_content($received_json) {

  $received_json = file_get_contents('php://input', TRUE);

  if (!$data = json_decode($received_json, TRUE)) {
    return FALSE;
  }
  return $data;
}

/**
 * Auth handler.
 */
function mock_cluster_rest_auth() {
  $method = $_SERVER['REQUEST_METHOD'];
  $message = '';
  $received_json = '';
  switch($method){
    case 'POST':
      if(!$data = mock_cluster_rest_request_content($received_json)){
        return array('error' => true, 'message' => 'Bad request. No data provided.');
      }
      
      if(!isset($data['method'])) {
        return array('error' => true, 'message' => 'method value is required. options: register, token.');
      }
      switch($data['method']) {
        case 'register':
            if(!isset($data['MAC'])){
              return array('error' => true, 'message' => 'MAC value is required.');
            }
            if(!isset($data['arch'])){
              return array('error' => true, 'message' => 'arch value is required.');
            }
            
            $agent = db_select('mock_agents', 'ma')
              ->fields('ma')
              ->condition('MAC', $data['MAC'])
              ->execute()
              ->fetchAssoc();

            if(empty($agent)){
              $secret = backdrop_random_key();
              $token = backdrop_random_key();
              $record = array(
                'MAC' => $data['MAC'],
                'arch' => $data['arch'],
                'secret' => $secret,
                'token' => $token,
                'token_expire' => REQUEST_TIME + 86400, 
                'created' => REQUEST_TIME,
                'access' => REQUEST_TIME,
                'status' => 0,
                'uid' => 0
              );
              backdrop_write_record('mock_agents', $record); 
              return array(
                'token' => $token,
                'secret' => $secret
              );
            } else {
              return array('error' => true, 'message' => 'Agent with this MAC is already registered.');
            }
          break;
        case 'token':
            if(!isset($data['MAC'])){
              return array('error' => true, 'message' => 'MAC value is required.');
            }
            if(!isset($data['arch'])){
              return array('error' => true, 'message' => 'arch value is required.');
            }

            $agent = db_select('mock_agents', 'ma')
              ->fields('ma')
              ->condition('MAC', $data['MAC'])
              ->condition('arch', $data['arch'])
              ->execute()
              ->fetchObject();

            if(empty($agent)){
              return array('error' => true, 'message' => 'Agent with this MAC is not registered.');
            }else {
              if(!_is_rest_authorised($agent, $received_json)) {
                return array('error' => true, 'message' => 'Authorization failed.');
              }

              return array(
                'token' => $agent->token,
                'expire' => $agent->token_expire
              );
            }
          break;
        default: 
          return array('error' => true, 'message' => 'method should be register or token only.');
      }
  }
}

/**
 * Check authorisation.
 */
function _is_rest_authorised($agent, $received_json){

  $method = $_SERVER['REQUEST_METHOD'];
  //'PUT', 'SEARCH', 'PATCH', 'POST'
  switch($method){
    case 'PUT':
    case 'SEARCH':
    case 'PATCH':
    case 'POST':
        echo $_SERVER['HTTP_SIGNATURE'];
        echo $received_json;
        if (isset($_SERVER['HTTP_SIGNATURE'])) {
          list($algorithm, $expected_hash) = explode('=', $_SERVER['HTTP_SIGNATURE'], 2);
          $actual_hash = hash_hmac($algorithm, $received_json, $agent->secret);
          if ($expected_hash === $actual_hash) {
            return TRUE;
          }
        }
      break;
    default:
      break;
  }
  return FALSE;
}

/**
 * Task handler.
 */
function mock_cluster_rest_task(){
  
}
