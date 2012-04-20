<?php
// $Id:$

/**
 * @file
 * Administrative page callbacks for the wnr module.
 */

/**
 * Implementation of hook_admin_settings() for configuring the module
 */
function webform_node_request_admin_settings_form(&$form_state) {
  $form = array();
  $form['webform_node_request']['webform_node_request_form_nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the form path'),
    '#default_value' => variable_get('webform_node_request_form_nid', ''),
    '#description' => t('Form path example: node/1')
  );

  $result = db_query('SELECT nid, title FROM {node} WHERE type = "webform"');
  while ($row = db_fetch_array($result)) {
    $options[$row['nid']] = $row['title'] . ' - ' . $row['nid'];
  }

  $form['webform_node_request']['webform_node_request_form_nids'] = array(
    '#type' => 'select',
    '#title' => t('Select webforms'),
    '#default_value' => variable_get('webform_node_request_form_nids', array()),
    '#options' => $options,
    '#multiple' => TRUE,
    '#size' => 5,
  );


  $form['webform_node_request']['webform_node_request_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter an alternate contact method'),
    '#default_value' => variable_get('webform_node_request_message', 'Request Quote'),
    '#description' => t('Enter the link text for the webform request.')
  );
  $form['webform_node_request']['webform_node_fieldset_legend'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter an alternate contact method'),
    '#default_value' => variable_get('webform_node_fieldset_legend', 'Request Quote On'),
    '#description' => t('Enter the link text for the webform request.')
  );
  $form['webform_node_request']['webform_node_request_content_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Requestable content types'),
    '#options' => webform_node_request_content_types(),
    '#default_value' => variable_get('webform_node_request_content_types', array()),
    '#description' => t('Enter the link text for the webform request.')
  );

  $form['webform_node_request']['webform_node_request_hidden_public'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the public webform form key'),
    '#default_value' => variable_get('webform_node_request_hidden_public', 'wnr_public_key'),
    '#description' => t('<strong>This is the publicly visible field</strong> - Useful for fields like "Interested in:"<br />This information can (and will by default) appear in submission emails. It is best to double check what information is being sent in the Webform settings for each Webform.')
  );

  $form['webform_node_request']['webform_node_request_hidden'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the hidden webform form key'),
    '#default_value' => variable_get('webform_node_request_hidden', 'wnr_hidden_key'),
    '#description' => t('<strong>This is the hidden field</strong><br />This information can (and will by default) appear in submission emails. It is best to double check what information is being sent in the Webform settings for each Webform.<br />Until http://drupal.org/node/1066374 is fixed, hidden fields won\'t be passed via email.'),
  );

  return system_settings_form($form);
}

function webform_node_request_content_types() {
  $types = node_get_types();
  $options = array();
  foreach ($types as $ct_obj) {
    $options[$ct_obj->type] = $ct_obj->name;
  }
  return $options;
}

function webform_node_request_admin_config_form(&$form_state) {
  $items = variable_get('webform_node_request_form_nids', array());
  $form = array();

  if (empty($items)) {
    $form['empty'] = array(
      '#title' => t('No webforms have been selected'),
      '#type' => 'markup',
      '#description' => t('No webforms have been selected'),
      '#value' => t('No webforms have been selected'),
    );
    return $form;
  }

  $selectable = variable_get('webform_node_request_content_types', array());
  $content_types = webform_node_request_content_types();

  $options = array();
  foreach ($content_types as $ct => $label) {
    if ($selectable[$ct] !== 0) {
      $options[$ct] = $label;
    }
  }

  // Build configuration form fields for each webform.
  foreach ($items as $nid) {
    $node = node_load($nid);

    if (!$node) {
      continue;
    }

    $defaults = variable_get('webform_node_request_config_types_' . $nid, array());

    $form['fs_' . $nid] = array(
      '#title' => t('Webform <strong>!webform_title</strong> configuration settings', array('!webform_title' => $node->title)),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => empty($defaults),
    );

    $form['fs_' . $nid]['webform_node_request_config_types_' . $nid] = array(
      '#type' => 'checkboxes',
      '#title' => t('Requestable content types'),
      '#options' => $options,
      '#default_value' => $defaults,
      '#description' => t('Select the content types that can be requested.')
    );

    $form['fs_' . $nid]['webform_node_request_config_types_view_' . $nid] = array(
      '#type' => 'textfield',
      '#title' => t('Webform data view'),
      '#default_value' => variable_get('webform_node_request_config_types_view_' . $nid, ''),
      '#description' => t('Enter the name of the view used to pass additional information in the request.')
    );

  }

  return system_settings_form($form);
}
