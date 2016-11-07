<?php
/**
 * @file
 * Access content pages provided by mock_cluster.
 */

/**
 * Menu callback for agents.
 */

function mock_cluster_agents(){
  $query = db_select('mock_agents', 'ma')->extend('AltPager');
  
  $agents = $query
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
    $row[] = array( 'data' => l($agent->identificationID, 'agents/' . $agent->identificationID ) );
    
    $title_arch = $agent->arch;
    if($arch = mock_arch_load($agent->arch)) {
      $title_arch = l($agent->arch, $arch->rpm_url);
    }
    $row[] = array( 'data' => $title_arch );
    $row[] = array( 'data' => $status );
    $row[] = array( 'data' => theme('username', array('account' => user_load($agent->uid))));
    
    if($is_admin) {
      if($agent->status == MOCK_AGENT_PENDING or $agent->status == MOCK_AGENT_BLOCKED) {
        $links['status'] = array(
          'title' => t('Activate'),
          'href' => "agents/" . $agent->identificationID . "/activate",
        );
      } else {
        $links['status'] = array(
          'title' => t('Block'),
          'href' => "agents/" . $agent->identificationID . "/block",
        );
      }
      $links['delete'] = array(
        'title' => t('Delete'),
        'href' => "agents/" . $agent->identificationID . "/delete",
      );
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
    $header = array(t('Identification ID'), t('ARCH'), t('Status'), t('Owner'), t('Operations'));
  } else {
    $header = array(t('Identification ID'), t('ARCH'), t('Status'), t('Owner'));
  }

  $output['pager_before'] = array(
    '#theme' => 'altpager',
  );
  $output['table'] = array(
    '#theme' => 'table__mock_cluster__agents',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No agents available yet.'),
  );
  $output['pager_after'] = array(
    '#theme' => 'altpager',
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
  $output .= '<dt>' . t('Identification ID'). '</dt>';
  $output .= '<dd>' . $agent->identificationID . '</dd>';

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
    '#markup' => t('Are you sure want to activate !identificationID agent?', array('!identificationID' => $agent->identificationID)),
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

  backdrop_set_message(t('Agent !identificationID activated!', array('!identificationID' => $agent->identificationID)));
  backdrop_goto('agents/' . $agent->identificationID);
}

/**
 * Activate agent handler.
 */
function mock_agent_block($form, &$form_state, $agent){

  $form_state['agent'] = $agent;

  $form['message'] = array(
    '#markup' => t('Are you sure want to block !identificationID agent?', array('!identificationID' => $agent->identificationID)),
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

  backdrop_set_message(t('Agent !identificationID blocked!', array('!identificationID' => $agent->identificationID)));
  backdrop_goto('agents/' . $agent->identificationID);

}

/**
 * Activate agent handler.
 */
function mock_agent_delete($form, &$form_state, $agent){

  $form_state['agent'] = $agent;

  $form['message'] = array(
    '#markup' => t('Are you sure want to delete !identificationID agent?', array('!identificationID' => $agent->identificationID)),
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

  backdrop_set_message(t('Agent !identificationID deleted!', array('!identificationID' => $agent->identificationID)));
  backdrop_goto('agents');
}
