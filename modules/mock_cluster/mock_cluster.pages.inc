<?php
/**
 * @file
 * Access content pages provided by mock_cluster.
 */

/**
 * Menu callback for agents.
 */

function mock_cluster_agents(){
  $agents = db_select('mock_agents', 'ma')
    ->fields('ma')
    ->orderBy('access', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  
  $is_admin = user_access('administer mock agents');
  
  foreach ($agents as $agent) {
    $row = array();
    $status = t('Pending');
    switch($agent->status) {
      case 1: 
        $status = t('Active');
        break;
      case 2: 
        $status = t('Disabled');
        break;
    }
    $row[] = array( 'data' => l($agent->MAC, 'agents/' . $agent->MAC ) );
    $row[] = array( 'data' => $agent->arch );
    $row[] = array( 'data' => $status );
    $row[] = array( 'data' => theme('username', array('account' => user_load($agent->uid))));
    
    if($is_admin) {
      $links['delete'] = array(
        'title' => t('Delete'),
        'href' => "agents/" . $agent->MAC . "/delete",
      );
      if($agent->status == MOCK_AGENT_PENDING or $agent->status == MOCK_AGENT_BLOCKED) {
        $links['status'] = array(
          'title' => t('Activate'),
          'href' => "agents/" . $agent->MAC . "/activate",
        );
      } else {
        $links['status'] = array(
          'title' => t('Block'),
          'href' => "agents/" . $agent->MAC . "/block",
        );
      }
      $row[] = array(
        'data' => array(
          '#type' => 'operations',
          '#links' => $links,
        ),
      );

    }
    $rows[] = $row;
  }
  if($is_admin) {
    $header = array(t('MAC address'), t('ARCH'), t('Status'), t('Owner'), t('Operations'));
  } else {
    $header = array(t('MAC address'), t('ARCH'), t('Status'), t('Owner'));
  }

  $output['table'] = array(
    '#theme' => 'table__mock_cluster__agents',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No agents available yet.'),
  );
  return $output;
}

function mock_cluster_agent($agent) {
  
  $status = t('Pending');
  switch($agent->status) {
    case 1: 
      $status = t('Active');
      break;
    case 2: 
      $status = t('Disabled');
      break;
  }
    
  $output = '<dl>';
  $output .= '<dt>' . t('MAC address'). '</dt>';
  $output .= '<dd>' . $agent->MAC . '</dd>';

  $output .= '<dt>' . t('ARCH'). '</dt>';
  $output .= '<dd>' . $agent->arch . '</dd>';

  $output .= '<dt>' . t('Status'). '</dt>';
  $output .= '<dd>' . $status . '</dd>';

  $output .= '<dt>' . t('Owner'). '</dt>';
  $output .= '<dd>' . theme('username', array('account' => user_load($agent->uid))) . '</dd>';
  
  

  $output .= '<dt>' . t('Created'). '</dt>';
  $output .= '<dd>' . format_date($agent->created) . '</dd>';

  $output .= '<dt>' . t('Last activity'). '</dt>';
  $output .= '<dd>' . format_date($agent->access) . '</dd>';

  $output .= '</dl>';
  return $output;
}

/**
 * Activate agent handler.
 */
function mock_agent_activate($form, &$form_state, $agent){

  $form_state['agent'] = $agent;

  $form['message'] = array(
    '#markup' => t('Are you sure want to activate !MAC agent?', array('!MAC' => $agent->MAC)),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Activate'),
  );

  return $form;
}

/**
 * Submit handler for the mock_agent_activate() form.
 */
function mock_agent_activate_submit($form, &$form_state) {
  global $user;
  $agent =  $form_state['agent'];
  db_update('mock_agents')
    ->fields(array('status' => 1, 'uid' => $user->uid, 'access' => REQUEST_TIME))
    ->condition('aid', $agent->aid)
    ->execute();

  backdrop_set_message(t('Agent !MAC activated!', array('!MAC' => $agent->MAC)));
  backdrop_goto('agents/' . $agent->MAC);
}

/**
 * Activate agent handler.
 */
function mock_agent_block($form, &$form_state, $agent){

  $form_state['agent'] = $agent;

  $form['message'] = array(
    '#markup' => t('Are you sure want to block !MAC agent?', array('!MAC' => $agent->MAC)),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Block'),
  );

  return $form;
}

/**
 * Submit handler for the mock_agent_block() form.
 */
function mock_agent_block_submit($form, &$form_state) {
  global $user;
  $agent =  $form_state['agent'];
  db_update('mock_agents')
    ->fields(array('status' => 2, 'uid' => $user->uid, 'access' => REQUEST_TIME))
    ->condition('aid', $agent->aid)
    ->execute();

  backdrop_set_message(t('Agent !MAC blocked!', array('!MAC' => $agent->MAC)));
  backdrop_goto('agents/' . $agent->MAC);

}

/**
 * Activate agent handler.
 */
function mock_agent_delete($form, &$form_state, $agent){

  $form_state['agent'] = $agent;

  $form['message'] = array(
    '#markup' => t('Are you sure want to delete !MAC agent?', array('!MAC' => $agent->MAC)),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

  return $form;
}

/**
 * Submit handler for the mock_agent_delete() form.
 */
function mock_agent_delete_submit($form, &$form_state) {
  global $user;
  $agent =  $form_state['agent'];
  db_delete('mock_agents')
    ->condition('aid', $agent->aid)
    ->execute();

  backdrop_set_message(t('Agent !MAC blocked!', array('!MAC' => $agent->MAC)));
  backdrop_goto('agents');
}
